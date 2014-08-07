/*
    Copyright (c) 2013, McAfee, Inc.
    
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    
    Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.
    
    Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.
    
    Neither the name of McAfee, Inc. nor the names of its contributors may be used
    to endorse or promote products derived from this software without specific prior
    written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>

#include <XMPHttp.h>

#define XME_HTTP_TIMEOUT 11300
#define XME_HTTP_SELECT 11301
#define XME_HTTP_SKREAD 11302
#define XME_HTTP_SKWRITE 11303
#define XME_HTTP_SETSOCKOPT 11003
#define XME_HTTP_DISCONNECT 11005
#define XME_HTTP_UNKNOWN_HOST 11006
#define XME_HTTP_CONNECT 11007

#define SOCK_STD_CONTIMEO 60
#define SOCK_STD_RCVTIMEO 60
#define SOCK_STD_SNDTIMEO 60
#define HTTP_DEFAULT_PORT 80
#define HTTPS_DEFAULT_PORT 443
#define CONTLEN_HEADER "Content-Length:"
#define CONTENC_HEADER "Transfer-Encoding:"
#define CHUNKED_ENCODED(s) (strcasecmp(s, "chunked") == 0)
#define XM_HTTP_HEADERS "XM_HTTP_HEADERS"
#define XM_HTTP_VERSION "XM_HTTP_VERSION"
#define XM_SKRECV_TIMEO "XM_NET_RECVTIMEO"
#define XM_SKSEND_TIMEO "XM_NET_SENDTIMEO"
#define XM_SKCONN_TIMEO "XM_NET_CONNTIMEO"


#define SSTREAM_BUFFER_SIZE 4096

#define URL_PROTO_HTTP  1
#define URL_PROTO_HTTPS 2

#if !defined(INADDR_NONE)
#define INADDR_NONE ((XmUInt32) -1)
#endif

#define URL_PARSE_INIT  { 0, NULL, 0, NULL, NULL }

#define PHTTP_MIN(a, b) ((a) < (b) ? (a): (b))
#define PHTTP_SKIPSPACE(p) for(; *(p) == ' ' || *(p) == '\t'; (p)++)
#define PHTTP_CSTRSIZE(s) (sizeof(s) - 1)

#define PHTTP_DBGPRINT(c, args) do { if ((c)->iPHttpDebug) XmPHttpDbgPrintf args; } while (0)

#define PHTTP_ENVGET(c, s) getenv(s)
#define PHTTP_ENVFREE(e) free(e)


#define INVALID_SOCKET (-1)
#define closesocket(s) close(s)
#define PHTTP_INPROGRESS(SockFd) (errno == EINPROGRESS || errno == EWOULDBLOCK)

typedef int SOCKET;


struct PHttpUrl
{
    int iProto;
    char const *pszHost;
    int iPort;
    char const *pszDoc;
    char const *pszUrl;
};

struct SStream
{
    XmPHttpCtx *pCtx;
    SOCKET SockFd;
    int iReadTimeo, iWriteTimeo;
    int iIndex;
    int iInBuffer;
    char Buffer[SSTREAM_BUFFER_SIZE];
};


static int XmPHttpDbgPrintf(char const *pszFmt, ...)
{
    int iPrintRes;
    va_list Args;

    va_start(Args, pszFmt);
    iPrintRes = vfprintf(stderr, pszFmt, Args);
    va_end(Args);

    return iPrintRes;
}

static int XmPHttpGetIntEnv(XmPHttpCtx *pCtx, char const *pszVar, int iDefault)
{
    int iValue = iDefault;
    char *pszValue;

    if ((pszValue = (char *) PHTTP_ENVGET(pCtx, pszVar)) != NULL)
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] %s environment is '%s'\n", pszVar, pszValue));

        iValue = atoi(pszValue);
        PHTTP_ENVFREE(pszValue);
    }

    return iValue;
}

static int XmPHttpSkBlocking(SOCKET SockFd, int iOn)
{
    int iFlags, iError;

    if ((iFlags = fcntl(SockFd, F_GETFL, 0)) == -1)
        return -XME_HTTP_SETSOCKOPT;
    if (iOn)
        iError = fcntl(SockFd, F_SETFL, iFlags & ~O_NONBLOCK);
    else
        iError = fcntl(SockFd, F_SETFL, iFlags | O_NONBLOCK);

    return iError == -1 ? -XME_HTTP_SETSOCKOPT: 0;
}

static int XmPHttpSkConnect(SOCKET SockFd, struct sockaddr *pAddr, int iAddrSize,
                            int iTimeout)
{
    int iError;

    if ((iError = XmPHttpSkBlocking(SockFd, 0)) < 0)
        return iError;
    if (connect(SockFd, pAddr, iAddrSize) != 0)
    {
        struct timeval TV;
        fd_set FdSet;

        if (!PHTTP_INPROGRESS(SockFd))
            return -XME_HTTP_CONNECT;
        FD_ZERO(&FdSet);
        FD_SET(SockFd, &FdSet);
        TV.tv_sec = iTimeout;
        TV.tv_usec = 0;
        if (select((int) SockFd + 1, NULL, &FdSet, NULL, &TV) < 0)
            return -XME_HTTP_SELECT;
        if (!FD_ISSET(SockFd, &FdSet))
            return -XME_HTTP_TIMEOUT;
    }

    return XmPHttpSkBlocking(SockFd, 1);
}

static int XmPHttpSkRead(SOCKET SockFd, void *pData, int iSize, int iTimeout)
{
    int iRead;
    struct timeval TV;
    fd_set FdSet;

    FD_ZERO(&FdSet);
    FD_SET(SockFd, &FdSet);
    TV.tv_sec = iTimeout;
    TV.tv_usec = 0;
    if (select((int) SockFd + 1, &FdSet, NULL, NULL, &TV) < 0)
        return -XME_HTTP_SELECT;
    if (!FD_ISSET(SockFd, &FdSet))
        return -XME_HTTP_TIMEOUT;
    if ((iRead = recv(SockFd, pData, iSize, 0)) < 0)
        return -XME_HTTP_SKREAD;
    else if (iRead == 0)
        return -XME_HTTP_DISCONNECT;

    return iRead;
}

static int XmPHttpSkWrite(SOCKET SockFd, void const *pData, int iSize, int iTimeout)
{
    int iWrite;
    struct timeval TV;
    fd_set FdSet;

    FD_ZERO(&FdSet);
    FD_SET(SockFd, &FdSet);
    TV.tv_sec = iTimeout;
    TV.tv_usec = 0;
    if (select((int) SockFd + 1, NULL, &FdSet, NULL, &TV) < 0)
        return -XME_HTTP_SELECT;
    if (!FD_ISSET(SockFd, &FdSet))
        return -XME_HTTP_TIMEOUT;
    if ((iWrite = send(SockFd, pData, iSize, 0)) < 0)
        return -XME_HTTP_SKWRITE;

    return iWrite;
}

static SOCKET XmPHttpSocket(XmPHttpCtx *pCtx)
{
    SOCKET SockFd;

    if ((SockFd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] Socket creation failed\n"));
        return INVALID_SOCKET;
    }

    return SockFd;
}

int XmPHttpInit(XmPHttpCtx *pCtx)
{
    pCtx->iPHttpDebug = 0;
    pCtx->iPHttpConnTimeo = XmPHttpGetIntEnv(pCtx, XM_SKCONN_TIMEO, SOCK_STD_CONTIMEO);

    PHTTP_DBGPRINT(pCtx, ("[phttp] Library initialization succeeded\n"));

    return 0;
}

void XmPHttpCleanup(XmPHttpCtx *pCtx)
{
    PHTTP_DBGPRINT(pCtx, ("[phttp] Library cleanup done\n"));
}

static struct SStream *XmPHttpStreamAttach(XmPHttpCtx *pCtx, SOCKET SockFd)
{
    struct SStream *pStream;

    if ((pStream = (struct SStream *) malloc(sizeof(struct SStream))) == NULL)
        return NULL;

    pStream->pCtx = pCtx;
    pStream->SockFd = SockFd;
    pStream->iIndex = 0;
    pStream->iInBuffer = 0;
    pStream->iReadTimeo = XmPHttpGetIntEnv(pCtx, XM_SKRECV_TIMEO, SOCK_STD_RCVTIMEO);
    pStream->iWriteTimeo = XmPHttpGetIntEnv(pCtx, XM_SKSEND_TIMEO, SOCK_STD_SNDTIMEO);

    return pStream;
}

static void XmPHttpStreamClose(struct SStream *pStream)
{
    closesocket(pStream->SockFd);
    PHTTP_DBGPRINT(pStream->pCtx, ("[phttp] Socket closed: sock=%u\n", pStream->SockFd));
    free(pStream);
}

static int XmPHttpStreamBRefil(struct SStream *pStream)
{
    XmPHttpCtx *pCtx = pStream->pCtx;

    PHTTP_DBGPRINT(pCtx, ("[phttp] Reading socket: sock=%u\n", pStream->SockFd));
    pStream->iIndex = 0;

    pStream->iInBuffer = XmPHttpSkRead(pStream->SockFd, pStream->Buffer,
                                       SSTREAM_BUFFER_SIZE, pStream->iReadTimeo);

    PHTTP_DBGPRINT(pCtx, ("[phttp] Socket read done: sock=%u recv=%d\n", pStream->SockFd,
                          pStream->iInBuffer));
    if (pStream->iInBuffer < 0)
        PHTTP_DBGPRINT(pCtx, ("[phttp] error is %d\n", (unsigned int) -pStream->iInBuffer));

    return pStream->iInBuffer;
}

static char *XmPHttpRdLine(char *pszLine, int iSize, struct SStream *pStream)
{
    int iLnSize, iTxSize;
    XmPHttpCtx *pCtx = pStream->pCtx;
    char *pszNL;

    for (iLnSize = 0, iSize--; iLnSize < iSize;)
    {
        if (pStream->iIndex >= pStream->iInBuffer &&
            XmPHttpStreamBRefil(pStream) <= 0)
            break;

        if ((pszNL = (char *) memchr(pStream->Buffer + pStream->iIndex, '\n',
                                     pStream->iInBuffer - pStream->iIndex)) != NULL)
        {
            iTxSize = PHTTP_MIN(iSize - iLnSize,
                                (int) (pszNL - (pStream->Buffer + pStream->iIndex) + 1));
            memcpy(pszLine + iLnSize, pStream->Buffer + pStream->iIndex, iTxSize);
            pStream->iIndex += iTxSize;
            iLnSize += iTxSize;
            break;
        }
        else
        {
            iTxSize = PHTTP_MIN(iSize - iLnSize, pStream->iInBuffer - pStream->iIndex);
            memcpy(pszLine + iLnSize, pStream->Buffer + pStream->iIndex, iTxSize);
            pStream->iIndex += iTxSize;
            iLnSize += iTxSize;
        }
    }

    if (iLnSize == 0)
        return NULL;

    for (; iLnSize > 0 && (pszLine[iLnSize - 1] == '\r' ||
                           pszLine[iLnSize - 1] == '\n'); iLnSize--);
    pszLine[iLnSize] = '\0';

    PHTTP_DBGPRINT(pCtx, ("[phttp] Line read: sock=%u\n"
                          "\t'%s'\n", pStream->SockFd, pszLine));

    return pszLine;
}

static int XmPHttpRead(char *pData, int iSize, struct SStream *pStream)
{
    int iRead = 0, iRxSize;
    XmPHttpCtx *pCtx = pStream->pCtx;

    if (pStream->iIndex < pStream->iInBuffer)
    {
        iRxSize = PHTTP_MIN(iSize, pStream->iInBuffer - pStream->iIndex);
        memcpy(pData, pStream->Buffer + pStream->iIndex, iRxSize);
        pStream->iIndex += iRxSize;
        iRead += iRxSize;
    }
    while (iRead < iSize)
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] Reading socket: sock=%u\n", pStream->SockFd));

        iRxSize = XmPHttpSkRead(pStream->SockFd, pData + iRead,
                                iSize - iRead, pStream->iReadTimeo);

        PHTTP_DBGPRINT(pCtx, ("[phttp] Socket read done: sock=%u recv=%d\n", pStream->SockFd,
                              iRxSize));
        if (iRxSize < 0)
            break;
        iRead += iRxSize;
    }

    return iRead;
}

static int XmPHttpPrintf(struct SStream *pStream, char const *pszFmt, ...)
{
    int iSize, iSent, iTxSize;
    XmPHttpCtx *pCtx = pStream->pCtx;
    va_list Args;
    char szBuffer[4096];

    va_start(Args, pszFmt);

    iSize = vsnprintf(szBuffer, sizeof(szBuffer) - 1, pszFmt, Args);

    va_end(Args);

    PHTTP_DBGPRINT(pCtx, ("[phttp] Sending ...\n"
                          "(\n"
                          "%s"
                          ")\n", szBuffer));
    for (iSent = 0; iSent < iSize;)
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] Writing socket: sock=%u\n", pStream->SockFd));

        iTxSize = XmPHttpSkWrite(pStream->SockFd, szBuffer + iSent,
                                 iSize - iSent, pStream->iWriteTimeo);

        PHTTP_DBGPRINT(pCtx, ("[phttp] Socket write done: sock=%u send=%d\n", pStream->SockFd,
                              iTxSize));
        if (iTxSize < 0)
            break;
        iSent += iTxSize;
    }

    return iSize;
}

static int XmPHttpWrite(char const *pData, int iSize, struct SStream *pStream)
{
    int iSent, iTxSize;
    XmPHttpCtx *pCtx = pStream->pCtx;

    for (iSent = 0; iSent < iSize;)
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] Writing socket: sock=%u\n", pStream->SockFd));

        iTxSize = XmPHttpSkWrite(pStream->SockFd, pData + iSent,
                                 iSize - iSent, pStream->iWriteTimeo);

        PHTTP_DBGPRINT(pCtx, ("[phttp] Socket write done: sock=%u send=%d\n", pStream->SockFd,
                              iTxSize));
        if (iTxSize < 0)
            break;
        iSent += iTxSize;
    }

    return iSize;
}

static struct SStream *XmPHttpConnect(XmPHttpCtx *pCtx, char const *pszServer, int iPort)
{
    int iError;
    SOCKET SockFd;
    struct hostent *pHE;
    struct SStream *pStream;
    struct in_addr InAddr;
    struct sockaddr_in SAIn;

    PHTTP_DBGPRINT(pCtx, ("[phttp] Resolving server name: server='%s'\n", pszServer));
    if ((InAddr.s_addr = inet_addr(pszServer)) == INADDR_NONE)
    {
        if ((pHE = gethostbyname(pszServer)) == NULL)
        {
            PHTTP_DBGPRINT(pCtx, ("[phttp] Server name resolve error: server='%s'\n", pszServer));
            return NULL;
        }

        memcpy(&InAddr.s_addr, pHE->h_addr_list[0], pHE->h_length);
    }
    PHTTP_DBGPRINT(pCtx, ("[phttp] Server name resolved: server='%s'\n", pszServer));

    if ((SockFd = XmPHttpSocket(pCtx)) == INVALID_SOCKET)
        return NULL;

    memset(&SAIn, 0, sizeof(SAIn));
    memcpy(&SAIn.sin_addr, &InAddr.s_addr, 4);
    SAIn.sin_port = htons((short int) iPort);
    SAIn.sin_family = AF_INET;

    PHTTP_DBGPRINT(pCtx, ("[phttp] Connecting to remote server: server='%s'\n", pszServer));
    if ((iError = XmPHttpSkConnect(SockFd, (struct sockaddr *) &SAIn, sizeof(SAIn),
                                   pCtx->iPHttpConnTimeo)) < 0)
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] Connect failed: server='%s'\n", pszServer));
        closesocket(SockFd);
        return NULL;
    }
    PHTTP_DBGPRINT(pCtx, ("[phttp] Connect succeeded: server='%s'\n", pszServer));

    if ((pStream = XmPHttpStreamAttach(pCtx, SockFd)) == NULL)
    {
        closesocket(SockFd);
        return NULL;
    }

    return pStream;
}

static int XmPHttpSendRequest(struct SStream *pStream, char const *pszServer,
                              char const *pszReq, char const *pszDoc,
                              char const * const *ppszHdrs,
                              XmHttpCallbacks *pHCB, void *pPrivate)
{
    int i, iCurr;
    long lReqSize, lRead;
    XmPHttpCtx *pCtx = pStream->pCtx;
    char *pszHttpVer;
    char TxBuff[512];

    if ((lReqSize = pHCB->pfGetSize(pPrivate)) < 0)
        return -1;
    PHTTP_DBGPRINT(pCtx, ("[phttp] Outbound data length retrieved: size=%ld\n",
                          lReqSize));

    pszHttpVer = (char *) PHTTP_ENVGET(pCtx, XM_HTTP_VERSION);
    if (XmPHttpPrintf(pStream, "%s %s %s\r\n", pszReq, pszDoc,
                      pszHttpVer != NULL ? pszHttpVer: "HTTP/1.0") < 0 ||
        XmPHttpPrintf(pStream, "Host: %s\r\n", pszServer) < 0)
    {
        PHTTP_ENVFREE(pszHttpVer);
        return -1;
    }
    PHTTP_ENVFREE(pszHttpVer);
    if (ppszHdrs)
        for (i = 0; ppszHdrs[i]; i++)
            if (XmPHttpPrintf(pStream, "%s\r\n", ppszHdrs[i]) < 0)
                return -1;

    if (XmPHttpPrintf(pStream, "Content-Length: %ld\r\n\r\n", lReqSize) < 0)
        return -1;

    PHTTP_DBGPRINT(pCtx, ("[phttp] Reading and sending outbound data: sock=%u\n",
                          pStream->SockFd));
    for (lRead = 0; lRead < lReqSize;)
    {
        iCurr = (int) PHTTP_MIN(lReqSize - lRead, sizeof(TxBuff));
        if (pHCB->pfRead(pPrivate, TxBuff, iCurr) != iCurr ||
            XmPHttpWrite(TxBuff, iCurr, pStream) != iCurr)
            return -1;
        lRead += iCurr;
    }
    PHTTP_DBGPRINT(pCtx, ("[phttp] Outbound data sent: sock=%u\n", pStream->SockFd));

    return 0;
}

static void XmPHttpParseInit(struct PHttpUrl *pPU)
{
    memset(pPU, 0, sizeof(*pPU));
}

static int XmPHttpParseUrl(XmPHttpCtx *pCtx, struct PHttpUrl *pPU, char const *pszUrl)
{
    char *pszDUrl = strdup(pszUrl), *pszTmp;

    if (pszDUrl == NULL)
    {
        return -1;
    }

    memset(pPU, 0, sizeof(*pPU));
    pPU->pszUrl = pszDUrl;
    if (!strncasecmp(pszDUrl, "http://", 7))
        pPU->iProto = URL_PROTO_HTTP, pszDUrl += 7, pPU->iPort = HTTP_DEFAULT_PORT;
    else if (!strncasecmp(pszDUrl, "https://", 8))
        pPU->iProto = URL_PROTO_HTTPS, pszDUrl += 8, pPU->iPort = HTTPS_DEFAULT_PORT;
    else
        pPU->iProto = URL_PROTO_HTTP, pPU->iPort = HTTP_DEFAULT_PORT;

    pPU->pszHost = pszDUrl;

    if ((pszTmp = strchr(pszDUrl, ':')) != NULL)
    {
        *pszTmp++ = '\0';
        if (!isdigit(*pszTmp))
        {
            free((char *) pPU->pszUrl);
            memset(pPU, 0, sizeof(*pPU));
            return -1;
        }
        pPU->iPort = atoi(pszTmp);
        pszDUrl = pszTmp;
    }

    if ((pszTmp = strchr(pszDUrl, '/')) != NULL)
    {
        pPU->pszDoc = strdup(pszTmp);
        *pszTmp++ = '\0';
    }
    else
        pPU->pszDoc = strdup("/");

    if (pPU->pszDoc == NULL)
    {
        free((char *) pPU->pszUrl);
        memset(pPU, 0, sizeof(*pPU));
        return -1;
    }

    return 0;
}

static void XmPHttpParseFree(struct PHttpUrl *pPU)
{
    free((char *) pPU->pszUrl);
    free((char *) pPU->pszDoc);
    memset(pPU, 0, sizeof(*pPU));
}

static int XmPHttpReadChunked(struct SStream *pStream, XmHttpCallbacks *pHCB,
                              void *pPrivate)
{
    int iCkSize;
    // XmPHttpCtx *pCtx = pStream->pCtx;
    char RxBuff[512];

    do
    {
        if (!XmPHttpRdLine(RxBuff, sizeof(RxBuff) - 1, pStream))
            return -1;
        if (!sscanf(RxBuff, "%d", &iCkSize))
            return -1;
        if (iCkSize)
        {
            int iSize, iRead;

            for (iSize = 0; iSize < iCkSize;)
            {
                iRead = PHTTP_MIN(iCkSize - iSize, sizeof(RxBuff));
                if (XmPHttpRead(RxBuff, iRead, pStream) != iRead)
                    return -1;
                if (pHCB && pHCB->pfWrite &&
                    pHCB->pfWrite(pPrivate, RxBuff, iRead) != iRead)
                    return -1;
                iSize += iRead;
            }
        }
    } while (iCkSize);
    while (XmPHttpRdLine(RxBuff, sizeof(RxBuff) - 1, pStream))
        if (strlen(RxBuff) == 0)
            break;

    return 0;
}

static int XmPHttpReadResponse(struct SStream *pStream, XmHttpCallbacks *pHCB,
                               void *pPrivate)
{
    int iHttpRCode, iChunked = 0, iValidRCode;
    long lContLen = -1;
    char *pszValue;
    XmPHttpCtx *pCtx = pStream->pCtx;
    char szBuffer[512];

    if (!XmPHttpRdLine(szBuffer, sizeof(szBuffer) - 1, pStream))
        return -1;

    PHTTP_DBGPRINT(pCtx, ("[phttp] HTTP response: sock=%u resp='%s'\n", pStream->SockFd, szBuffer));
    if (!sscanf(szBuffer, "%*s %d %*s", &iHttpRCode))
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] Wrong HTTP response: sock=%u resp='%s'\n", pStream->SockFd, szBuffer));
        return -1;
    }

    // printf("[XMPHTTP] : HTTP CODE = %d\n", iHttpRCode);
    iValidRCode = iHttpRCode == 200 || iHttpRCode == 204;
    if ((iHttpRCode >= 100 && iHttpRCode < 200) || iHttpRCode == 204 || iHttpRCode == 304)
        lContLen = 0;

    while (XmPHttpRdLine(szBuffer, sizeof(szBuffer) - 1, pStream) != NULL)
    {
        PHTTP_DBGPRINT(pCtx, ("[phttp] HTTP header: sock=%u hdr='%s'\n", pStream->SockFd, szBuffer));
        if (strlen(szBuffer) == 0)
            break;
        if (!strncasecmp(szBuffer, CONTLEN_HEADER, PHTTP_CSTRSIZE(CONTLEN_HEADER)))
        {
            pszValue = szBuffer + PHTTP_CSTRSIZE(CONTLEN_HEADER);
            PHTTP_SKIPSPACE(pszValue);
            lContLen = atol(pszValue);
        }
        else if (!strncasecmp(szBuffer, CONTENC_HEADER, PHTTP_CSTRSIZE(CONTENC_HEADER)))
        {
            pszValue = szBuffer + PHTTP_CSTRSIZE(CONTENC_HEADER);
            PHTTP_SKIPSPACE(pszValue);
            iChunked = CHUNKED_ENCODED(pszValue);
        }
    }
    if (lContLen >= 0)
    {
        int iRead;
        long lSize;

        for (lSize = 0; lSize < lContLen;)
        {
            iRead = (int) PHTTP_MIN(lContLen - lSize, sizeof(szBuffer));
            if (XmPHttpRead(szBuffer, iRead, pStream) != iRead)
                return -1;
            if (iValidRCode && pHCB->pfWrite &&
                pHCB->pfWrite(pPrivate, szBuffer, iRead) != iRead)
                return -1;
            lSize += iRead;
        }
    }
    else if (iChunked)
    {
        if (XmPHttpReadChunked(pStream, iValidRCode ? pHCB: NULL, pPrivate) < 0)
            return -1;
    }
    else
    {
        int iRead;
        long lSize;

        for (lSize = 0;;)
        {
            if ((iRead = XmPHttpRead(szBuffer, sizeof(szBuffer), pStream)) <= 0)
                break;
            if (iValidRCode && pHCB->pfWrite &&
                pHCB->pfWrite(pPrivate, szBuffer, iRead) != iRead)
                return -1;
            lSize += iRead;
        }
    }

    return iValidRCode ? 0: -1;
}

static char **XmPHttpHdrSplit(char const *pszStr)
{
    int i, j, k, iNumStrs;
    void *pMemBlk;
    char **ppszPtrs;
    char *pszDupStr;

    for (i = 0, iNumStrs = 1; pszStr[i]; i++)
        if (pszStr[i] == '|')
            iNumStrs++;
    if ((pMemBlk = malloc(i + 1 +
                          (iNumStrs + 1) * sizeof(char *))) == NULL)
        return NULL;
    ppszPtrs = (char **) pMemBlk;
    pszDupStr = (char *) pMemBlk + (iNumStrs + 1) * sizeof(char *);
    memcpy(pszDupStr, pszStr, i + 1);
    for (i = j = 0, k = -1; pszDupStr[i]; i++)
        if (pszDupStr[i] == '|')
        {
            if (k >= 0)
                ppszPtrs[j++] = pszDupStr + k;
            pszDupStr[i] = '\0';
            k = -1;
        }
        else if (k < 0)
            k = i;
    if (k >= 0)
        ppszPtrs[j++] = pszDupStr + k;
    ppszPtrs[j] = NULL;

    return ppszPtrs;
}

int XmPHttpExec(XmPHttpCtx *pCtx, char const *pszMethod, char const *pszUrl,
                XmHttpCallbacks *pHCB, void *pPrivate)
{
    char *pszEnv = NULL;
    char **ppszHdrs = NULL;
    struct SStream *pStream;
    struct PHttpUrl PU;

    XmPHttpParseInit(&PU);
    if (XmPHttpParseUrl(pCtx, &PU, pszUrl) < 0)
        return -1;
    if ((pStream = XmPHttpConnect(pCtx, PU.pszHost, PU.iPort)) == NULL)
    {

        XmPHttpParseFree(&PU);
        return -1;
    }
    if ((pszEnv = (char *) PHTTP_ENVGET(pCtx, XM_HTTP_HEADERS)) != NULL &&
        (ppszHdrs = XmPHttpHdrSplit(pszEnv)) == NULL)
    {

        PHTTP_ENVFREE(pszEnv);
        XmPHttpStreamClose(pStream);
        XmPHttpParseFree(&PU);
        return -1;
    }
    PHTTP_ENVFREE(pszEnv);

    if (XmPHttpSendRequest(pStream, PU.pszHost, pszMethod, PU.pszDoc,
                           (char const * const *) ppszHdrs, pHCB, pPrivate) < 0 ||
        XmPHttpReadResponse(pStream, pHCB, pPrivate) < 0)
    {

        if (ppszHdrs != NULL)
            free(ppszHdrs);
        XmPHttpStreamClose(pStream);
        XmPHttpParseFree(&PU);
        return -1;
    }
    free(ppszHdrs);
    XmPHttpStreamClose(pStream);
    XmPHttpParseFree(&PU);

    return 0;
}

