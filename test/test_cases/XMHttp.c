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

#include <malloc.h>

#include "XMPHttp.h"


typedef struct XmHttpCtx_struct
{
    XmPHttpCtx PCtx;
} XmHttpCtx;


XM_HTTP_HANDLE XmHttpOpen()
{
    XmHttpCtx *pCtx;

    if ((pCtx = (XmHttpCtx *) malloc(sizeof(XmHttpCtx))) == NULL)
        return INVALID_XM_HTTP_HANDLE;
    if (XmPHttpInit(&pCtx->PCtx) < 0)
    {
        free(pCtx);
        return INVALID_XM_HTTP_HANDLE;
    }

    return (XM_HTTP_HANDLE) pCtx;
}


void XmHttpClose(XM_HTTP_HANDLE hHTTP)
{
    XmHttpCtx *pCtx = (XmHttpCtx *) hHTTP;

    XmPHttpCleanup(&pCtx->PCtx);
    free(pCtx);
}


int XmHttpExec(XM_HTTP_HANDLE hHTTP, char const *pszMethod, char const *pszURL,
               XmHttpCallbacks *pHCB, void *pPrivate)
{
    XmHttpCtx *pCtx = (XmHttpCtx *) hHTTP;

    return XmPHttpExec(&pCtx->PCtx, pszMethod, pszURL, pHCB, pPrivate);
}

