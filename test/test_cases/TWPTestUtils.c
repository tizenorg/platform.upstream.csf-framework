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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include "TWPImpl.h"
#include "XMHttp.h"
#include "TWPTest.h"


#if !defined(MIN)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif


static void ReportTestCase(TestCase *pCtx);
static void CallSys(const char *pszCmd);
static void PutTestRoot(char *pszRoot);
static char *GetTestRoot(void);
static char *GetBackupDir(void);
static void PutBackupDir(char *pszBackupDir);


int TestCasesCount = 0;
int Success = 0;
int Failures = 0;
jmp_buf WPJmpBuf;
TWPResponseHandle hAResponse = NULL;


/**
 * Output for test case result.
 */
static void ReportTestCase(TestCase *pCtx)
{
    char *pszTmp;

    LOG_OUT("@@@@@@@@@@@@@@@@@@@@@@@@\n");

    LOG_OUT("@ID: TC_SEC_WP_%s\n", pCtx->szAPIName);
    pszTmp = strchr(pCtx->szAPIName, '_');
    *pszTmp = 0;
    LOG_OUT("@API Name: %s\n", pCtx->szAPIName);
    *pszTmp = '_';
}


/**
 * Test case constructor.
 */
void TestCaseCtor(TestCase *pCtx, const char *pszAPI)
{

    strncpy(pCtx->szAPIName, pszAPI, sizeof(pCtx->szAPIName) - 1);
}


/**
 * Test case destructor.
 */
void TestCaseDtor(TestCase *pCtx)
{

    ReportTestCase(pCtx);
    Success++;
    
    extern TRequest Request;
    if (Request.ResponseBody != NULL)
        free(Request.ResponseBody);

    Request.pszUrl = NULL;
    Request.ResponseLength = 0;
    Request.ResponseBody = NULL;
    
    Request.pData = NULL;
    Request.uLength = 0;
    Request.uRead = 0;
    Request.ResponseBytesRead = 0;
}


void BackupEngine()
{
    char *pszRoot = GetTestRoot();

    if (pszRoot != NULL)
    {
        char szCommand[1024];

        sprintf(szCommand, "mkdir %s/backup 2>/dev/null", pszRoot);
        CallSys(szCommand);

        sprintf(szCommand, "cp -f /opt/usr/share/sec_plugin/libwpengine.so %s/backup", pszRoot);
        CallSys(szCommand);

        PutTestRoot(pszRoot);
    }
}


void RestoreEngine()
{
    char *pszRoot = GetTestRoot();

    if (pszRoot != NULL)
    {
        char szCommand[1024];

        sprintf(szCommand, "cp -f %s/backup/libwpengine.so /opt/usr/share/sec_plugin/", pszRoot);
        CallSys(szCommand);

        PutTestRoot(pszRoot);
    }
}


void RemoveEngine()
{
    char *pszRoot = GetTestRoot();

    BackupEngine();
    if (pszRoot != NULL)
    {
        char szCommand[1024];
        sprintf(&szCommand, "rm -f /opt/usr/share/sec_plugin/libwpengine.so");
        CallSys(szCommand);
    }
}


long GenerateRandomNumber()
{

	return rand();
}


TWP_RESULT CbSetUrl(struct TWPRequest *pRequest, const char *pszUrl, unsigned int uLength)
{
    TRequest *pCtx = (TRequest *) pRequest;

    // LOG_OUT("url is: %s\n", pszUrl);
    if (pCtx->pszUrl != NULL)
        free((void *) pCtx->pszUrl);
    pCtx->pszUrl = strdup(pszUrl);

    return pCtx->pszUrl ? TWP_SUCCESS : TWP_NOMEM;
}


TWP_RESULT CbSetMethod(struct TWPRequest *pRequest, TWPSubmitMethod Method)
{

    return Method == TWPPOST ? TWP_SUCCESS : TWP_INVALID_PARAMETER;
}


static int CbHttpWrite(void *pPrivate, void const *pData, int iSize)
{
    TRequest *pCtx = (TRequest *) pPrivate;
    char *pTmp = NULL;

    // LOG_OUT("[http] recv data\n");
    pTmp = (char *) realloc(pCtx->ResponseBody, pCtx->ResponseLength + iSize);
    if (pTmp == NULL)
    {
        LOG_OUT("failed to alloc mem\n");
        return 0;
    }

    pCtx->ResponseBody = pTmp;
    memcpy(pCtx->ResponseBody + pCtx->ResponseLength, pData, iSize);
    pCtx->ResponseLength += iSize;

    return iSize;
}


static int CbHttpRead(void *pPrivate, void *pData, int iSize)
{
    TRequest *pCtx = (TRequest *) pPrivate;
    unsigned int uToRead = MIN(iSize, pCtx->uLength - pCtx->uRead);

    // LOG_OUT("[http] send data\n");

    memcpy(pData, pCtx->pData + pCtx->uRead, uToRead);

    return (int) uToRead;
}


static long CbHttpGetSize(void *pPrivate)
{
    TRequest *pCtx = (TRequest *) pPrivate;

    // LOG_OUT("[http] get size\n");

    return (long) (pCtx->uLength - pCtx->uRead);
}


static TWP_RESULT HttpSend(TRequest *pCtx, const void *pData, unsigned int uLength)
{
    int iRet;
    XmHttpCallbacks HttpCb;

    if (pCtx->hHttp == INVALID_XM_HTTP_HANDLE)
    {
        pCtx->hHttp = XmHttpOpen();
        if (pCtx->hHttp == NULL)
            return TWP_NOMEM;
    }

    HttpCb.pfWrite = CbHttpWrite;
    HttpCb.pfRead = CbHttpRead;
    HttpCb.pfGetSize = CbHttpGetSize;

    pCtx->pData = (char *) pData;
    pCtx->uLength = uLength;

    iRet = XmHttpExec(pCtx->hHttp, "POST", pCtx->pszUrl, &HttpCb, pCtx);

    return iRet == 0 ? TWP_SUCCESS : TWP_ERROR;
}


TWP_RESULT CbSend(struct TWPRequest *pRequest, TWPResponseHandle hResponse,
                  const void *pData, unsigned int uLength)
{

    // For a-sync call
    hAResponse = hResponse;
    return HttpSend((TRequest *) pRequest, pData, uLength);
}


TWP_RESULT CbRecv(struct TWPRequest *pRequest, void *pBuffer, unsigned int uBufferLength,
                  unsigned int *puLength)
{
    TRequest* pCtx = (TRequest*) pRequest;
    size_t BytesToCopy = 0;

    if ( pCtx->ResponseBytesRead < pCtx->ResponseLength )
    {
    	BytesToCopy = uBufferLength < (pCtx->ResponseLength - pCtx->ResponseBytesRead) ? uBufferLength : (pCtx->ResponseLength - pCtx->ResponseBytesRead);
    	memcpy(pBuffer, (void*) (pCtx->ResponseBody + pCtx->ResponseBytesRead), BytesToCopy);
    	pCtx->ResponseBytesRead += BytesToCopy;
    }

    *puLength = (int) BytesToCopy;

    return TWP_SUCCESS;
}


static void CallSys(const char *pszCmd)
{
    int iRet = 0;
    iRet = system(pszCmd);
    if (iRet != 0)
    {
        // LOG_OUT("failed to exe command: %x\n", (int) pszCmd);
    }
}


/**
 * Test framework helper function: get content files' root path.
 */
static char *GetTestRoot(void)
{
    int iLen, iEnvLen;
    char *pszRoot = NULL, *pszEnv = getenv("TWP_CONTENT_PATH");

    if (pszEnv != NULL &&
        (iEnvLen = strlen(pszEnv)) > 0)
    {
        iLen = iEnvLen;
        iLen += 64; /* Reserved 64 bytes for PID. */
        iLen += strlen(TWP_TEST_CONTENT_DIR);
        pszRoot = (char *) calloc(iLen + 1, sizeof(char));
        if (pszRoot != NULL)
        {
            if (pszEnv[iEnvLen - 1] != '/')
                snprintf(pszRoot, iLen, "%s/%d/%s", pszEnv, (int) getpid(),
                         TWP_TEST_CONTENT_DIR);
            else
                snprintf(pszRoot, iLen, "%s%d/%s", pszEnv, (int) getpid(),
                         TWP_TEST_CONTENT_DIR);
        }
    }
    else
    {
        iLen = sizeof("./") + 64; /* Reserved 64 bytes for PID. */
        pszRoot = (char *) calloc(iLen + 1, sizeof(char));
        if (pszRoot != NULL)
        {
            snprintf(pszRoot, iLen, "./%d", (int) getpid());
        }
    }

    return pszRoot;
}


static void PutTestRoot(char *pszRoot)
{

    if (pszRoot != NULL)
        free(pszRoot);
}


int CreateTestDirs(void)
{
    int iLen, iRet = -1;
    char *pszCommand, *pszRoot = GetTestRoot(), *pszEnv, *pszBackup;

    if (pszRoot != NULL)
    {
        pszBackup = GetBackupDir();
        if (pszBackup != NULL)
        {
            iLen = MAX(strlen(pszRoot) * 2, strlen(pszBackup));
            iLen += 64; /* Reserved for "mkdir -p", "cp -f " */
            pszCommand = (char *) calloc(iLen + 1, sizeof(char));
            if (pszCommand != NULL)
            {
                snprintf(pszCommand, iLen, "mkdir -p %s", pszRoot);
                CallSys(pszCommand);
                pszCommand[0] = 0;

                snprintf(pszCommand, iLen, "mkdir -p %s", pszBackup);
                CallSys(pszCommand);
                pszCommand[0] = 0;

                pszEnv = getenv("TWP_CONTENT_PATH");
                if (pszEnv == NULL)
                    pszEnv = "./";
                if (pszEnv[strlen(pszEnv) - 1] == '/')
                    snprintf(pszCommand, iLen, "cp -f %s* %s 2>/dev/null", pszEnv, pszRoot);
                else
                    snprintf(pszCommand, iLen, "cp -f %s/* %s 2>/dev/null", pszEnv, pszRoot);
                CallSys(pszCommand);

                free(pszCommand);

                iRet = 0;
            }
            PutBackupDir(pszBackup);
        }
        PutTestRoot(pszRoot);
    }

    return iRet;
}


void DestoryTestDirs(void)
{
    int iLen, iEnvLen;
    char *pszCommand, *pszEnv = getenv("TWP_CONTENT_PATH");

    if (pszEnv == NULL || strlen(pszEnv) == 0)
        pszEnv = "./";
    iEnvLen = strlen(pszEnv);
    iLen = iEnvLen;
    iLen += 72; /* Reserved for "rm -rf" and PID */
    pszCommand = (char *) calloc(iLen + 1, sizeof(char));
    if (pszCommand != NULL)
    {
        if (pszEnv[iEnvLen - 1] == '/')
            snprintf(pszCommand, iLen, "rm -rf %s%d", pszEnv, (int) getpid());
        else
            snprintf(pszCommand, iLen, "rm -rf %s/%d", pszEnv, (int) getpid());
        CallSys(pszCommand);
        free(pszCommand);
    }
}


static char *GetBackupDir(void)
{
    int iLen, iEnvLen;
    char *pszEnv = getenv("TWP_CONTENT_PATH"), *pszPath;

    if (pszEnv == NULL || strlen(pszEnv) == 0)
        pszEnv = "./";
    iEnvLen = strlen(pszEnv);
    iLen = iEnvLen;
    iLen += strlen(TWP_BACKUP_CONTENT_DIR);
    iLen += 64; /* Reserved for slash char and PID. */
    pszPath = (char *) calloc(iLen + 1, sizeof(char));

    if (pszPath)
    {
        if (pszEnv[iEnvLen - 1] == '/')
            snprintf(pszPath, iLen, "%s%d/%s", pszEnv, (int) getpid(),
                     TWP_BACKUP_CONTENT_DIR);
        else
            snprintf(pszPath, iLen, "%s/%d/%s", pszEnv, (int) getpid(),
                     TWP_BACKUP_CONTENT_DIR);
    }

    return pszPath;
}


static void PutBackupDir(char *pszBackupDir)
{

    if (pszBackupDir != NULL)
        free(pszBackupDir);
}

