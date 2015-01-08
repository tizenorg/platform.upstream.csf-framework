/*
    Copyright (c) 2014, McAfee, Inc.
    
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
#include <unistd.h>
#include <dlfcn.h>
#include <malloc.h>
#include <pthread.h>
#include <string.h>

#include "Debug.h"
#include "TCSImpl.h"
#include "TCSErrorCodes.h"


#define TCS_CONSTRUCT_ERRCODE(m, e) (((m) << 24) | (e))

#define PLUGIN_PATH "/opt/usr/share/sec_plugin/libengine.so"


typedef TCSLIB_HANDLE (*FuncLibraryOpen)(void);
typedef int (*FuncLibraryClose)(TCSLIB_HANDLE hLib);
typedef TCSErrorCode (*FuncGetLastError)(TCSLIB_HANDLE hLib);
typedef int (*FuncScanData)(TCSLIB_HANDLE hLib, TCSScanParam *pParam, TCSScanResult *pResult);
typedef int (*FuncScanFile)(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                            int iAction, int iCompressFlag, TCSScanResult *pResult);
typedef int (*FuncScanFileEx)(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                            int iAction, int iCompressFlag,
                            void *pPrivate, int (*pfCallBack)(void *pPrivate, int iReason, void *pParam),
                            TCSScanResult *pResult);
typedef char const* (*FuncGetVersion)();
typedef char const* (*FuncGetInfo)();

typedef struct PluginContext_struct
{
    TCSLIB_HANDLE hLib;
    void *pPlugin;
    FuncLibraryOpen pfLibraryOpen;
    FuncLibraryClose pfLibraryClose;
    FuncGetLastError pfGetLastError;
    FuncScanData pfScanData;
    FuncScanFile pfScanFile;
    FuncScanFileEx pfScanFileEx;
    FuncGetVersion pfGetVersion;
    FuncGetInfo pfGetInfo;
    pthread_mutex_t mutex;
} PluginContext;

typedef struct ThreadScanFileData_struct
{
    TCSLIB_HANDLE hLib;
    char* pszFileName;
    int iDataType;
    int iAction;
    int iCompressFlag;
    void *pPrivate;
    int (*pfCallBack)(void *pPrivate, int iReason, void *pParam);
} ThreadScanFileData;

typedef struct ThreadScanData_struct
{
    TCSLIB_HANDLE hLib;
    TCSScanParam *pParam;
} ThreadScanData;

static PluginContext *LoadPlugin(void);


TCSLIB_HANDLE TCSLibraryOpen(void)
{
    PluginContext *pCtx = NULL;

    DDBG("%s", "tcs lib open\n");
    pCtx = LoadPlugin();
    if (pCtx != NULL)
    {
        if (pCtx->pfLibraryOpen == NULL)
        {
            free(pCtx);
            return INVALID_TCSLIB_HANDLE;
        }
        DDBG("%s", "call to TCSPLibraryOpen\n");
        pCtx->hLib = (*pCtx->pfLibraryOpen)();
        if (pCtx->hLib == INVALID_TCSLIB_HANDLE)
        {
            DDBG("%s", "failed to open engine\n");
            if (pCtx->pPlugin != NULL)
                dlclose(pCtx->pPlugin);
            free(pCtx);
        }
        else
        {
            pthread_mutex_init(&(pCtx->mutex), NULL);
            return (TCSLIB_HANDLE) pCtx;
        }
    }

    return INVALID_TCSLIB_HANDLE;
}


int TCSGetVersion(TCSLIB_HANDLE hLib, TCSVerInfo *pVerInfo)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfGetVersion == NULL)
    {
        return -1;
    }

    if (pVerInfo == NULL)
    {
        return -1;
    }

    strncpy(pVerInfo->szFrameworkVer, TCS_FRAMEWORK_VERSION, TCS_VER_MAX);

    char const *pPluginVer = (*pCtx->pfGetVersion)();

    if (pPluginVer != NULL)
        strncpy(pVerInfo->szPluginVer, pPluginVer, TCS_VER_MAX - 1);
    else
        strncpy(pVerInfo->szPluginVer, "NULL", TCS_VER_MAX - 1);

    pVerInfo->szPluginVer[TCS_VER_MAX - 1] = '\0';

    DDBG("%s %s %s\n", "Framework|Plugin version = ",
              pVerInfo->szFrameworkVer, pVerInfo->szPluginVer);

    return 0;
}

int TCSGetInfo(TCSLIB_HANDLE hLib, char *pszInfo)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfGetInfo == NULL)
    {
        return -1;
    }

    if (pszInfo == NULL)
    {
        return -1;
    }

    char const *pszInfoPlugin = (*pCtx->pfGetInfo)();

    if (pszInfoPlugin != NULL)
        strncpy(pszInfo, pszInfoPlugin, TCS_META_MAX - 1);
    else
        strncpy(pszInfo, "NULL", TCS_META_MAX - 1);

    pszInfo[TCS_META_MAX - 1] = '\0';

    return 0;
}

int TCSLibraryClose(TCSLIB_HANDLE hLib)
{
    int iRet = -1;
    PluginContext *pCtx = NULL;

    if (hLib == INVALID_TCSLIB_HANDLE)
        return iRet;

    pCtx = (PluginContext *) hLib;
    if (pCtx->pfLibraryClose == NULL)
        return iRet;

    pthread_mutex_lock(&(pCtx->mutex));
    iRet = (*pCtx->pfLibraryClose)(pCtx->hLib);

    if (pCtx->pPlugin != NULL)
    {
        dlclose(pCtx->pPlugin);
        pCtx->pPlugin = NULL;
    }

    pthread_mutex_unlock(&(pCtx->mutex));
    pthread_mutex_destroy(&(pCtx->mutex));
    free(pCtx);

    return iRet;
}


TCSErrorCode TCSGetLastError(TCSLIB_HANDLE hLib)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfGetLastError == NULL)
    {
        return TCS_CONSTRUCT_ERRCODE(TCS_ERROR_MODULE_GENERIC,
                                     TCS_ERROR_NOT_IMPLEMENTED);
    }

    pthread_mutex_lock(&(pCtx->mutex));
    TCSErrorCode errorCode = (*pCtx->pfGetLastError)(pCtx->hLib);
    pthread_mutex_unlock(&(pCtx->mutex));

    return errorCode;
}


int TCSScanData(TCSLIB_HANDLE hLib, TCSScanParam *pParam, TCSScanResult *pResult)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfScanData == NULL)
    {
        return -1;
    }
    return (*pCtx->pfScanData)(pCtx->hLib, pParam, pResult);
}


int TCSScanFile(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                int iAction, int iCompressFlag, TCSScanResult *pResult)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfScanFile == NULL)
    {
        return -1;
    }
    return (*pCtx->pfScanFile)(pCtx->hLib, pszFileName, iDataType, iAction, iCompressFlag, pResult);
}

int TCSScanFileEx(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
                int iAction, int iCompressFlag, void *pPrivate,
                int (*pfCallBack)(void *pPrivate, int iReason, void *pParam),
                TCSScanResult *pResult)
{
    PluginContext *pCtx = (PluginContext *) hLib;

    if (pCtx == NULL || pCtx->pfScanFile == NULL)
    {
        return -1;
    }
    return (*pCtx->pfScanFileEx)(pCtx->hLib, pszFileName, iDataType, iAction, iCompressFlag,
                                pPrivate, pfCallBack, pResult);
}

int TCSScanAsyncCreateThread(void *pfWorkerFunc, void *pThreadData)
{
    pthread_t thread;
    pthread_attr_t attr;

    int rc = -1;
    do
    {
        if (pthread_attr_init(&attr) != 0)
            break;

        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
            break;

        if (pthread_create(&thread, &attr, pfWorkerFunc, pThreadData) != 0)
            break;

        if (pthread_attr_destroy(&attr) != 0)
            break;

         rc = 0;
     }
     while (0);

     return rc;
}

void *TCSScanDataAsyncWorker(void *pTData)
{
    ThreadScanData *pThreadData;
    pThreadData = (ThreadScanData *) pTData;

    PluginContext *pCtx = (PluginContext *) pThreadData->hLib;
    TCSScanParam *pTCSScanParam = pThreadData->pParam;

    int iRet = -1;

    do
    {
        if (pCtx == NULL || pCtx->pfScanFile == NULL)
        {
            DDBG("%s", "failed to open engine\n");
            pTCSScanParam->pfCallBack(pTCSScanParam->pPrivate, TCS_CB_SCANFINISH, NULL);
            break;
        }

        TCSScanResult Result;

        pthread_mutex_lock(&(pCtx->mutex));

        iRet = TCSScanData(pThreadData->hLib, pTCSScanParam, &Result);

        pthread_mutex_unlock(&(pCtx->mutex));

        if (iRet != 0)
            pTCSScanParam->pfCallBack(pTCSScanParam->pPrivate, TCS_CB_SCANFINISH, NULL);
        else
            pTCSScanParam->pfCallBack(pTCSScanParam->pPrivate, TCS_CB_SCANFINISH, &Result);
    }
    while(0);

    free(pThreadData->pParam);
    free(pThreadData);
    pThreadData = NULL;

    return NULL;
}

int TCSScanDataAsync(TCSLIB_HANDLE hLib, TCSScanParam *pParam)
{
    ThreadScanData *pThreadData = calloc(1, sizeof(ThreadScanData));

    int rc = -1;

    do
    {
        if (pThreadData == NULL)
            break;

        pThreadData->pParam = malloc(sizeof(TCSScanParam));

        if (pThreadData->pParam == 0)
            break;

        pThreadData->hLib = hLib;
        memcpy(pThreadData->pParam, pParam, sizeof(TCSScanParam));

        if (TCSScanAsyncCreateThread(TCSScanDataAsyncWorker, (void *) pThreadData) != 0)
            break;

        rc = 0;
    }
    while (0);

    if (rc == -1)
    {
        if (pThreadData != NULL)
        {
            free(pThreadData->pParam);
            free(pThreadData);
        }
    }

    return rc;
}

void *TCSScanFileAsyncWorker(void *pTData)
{
    ThreadScanFileData *pThreadData;
    pThreadData = (ThreadScanFileData *) pTData;

    PluginContext *pCtx = (PluginContext *) pThreadData->hLib;

    int iRet = -1;

    do
    {
        if (pCtx == NULL || pCtx->pfScanFile == NULL)
        {
            DDBG("%s", "failed to open engine\n");
            pThreadData->pfCallBack(pThreadData->pPrivate, TCS_CB_SCANFINISH, NULL);
            break;
        }

        TCSScanResult Result;

        pthread_mutex_lock(&(pCtx->mutex));

        iRet = TCSScanFileEx(pThreadData->hLib, pThreadData->pszFileName, pThreadData->iDataType,
                           pThreadData->iAction, pThreadData->iCompressFlag, pThreadData->pPrivate,
                           pThreadData->pfCallBack, &Result);

        pthread_mutex_unlock(&(pCtx->mutex));

        if (iRet != 0)
            pThreadData->pfCallBack(pThreadData->pPrivate, TCS_CB_SCANFINISH, NULL);
        else
            pThreadData->pfCallBack(pThreadData->pPrivate, TCS_CB_SCANFINISH, &Result);
    }
    while(0);

    free(pThreadData->pszFileName);
    free(pThreadData);
    pThreadData = NULL;

    return NULL;
}

int TCSScanFileAsync(TCSLIB_HANDLE hLib, char const *pszFileName, int iDataType,
        int iAction, int iCompressFlag, void *pPrivate, 
        int (*pfCallBack)(void *pPrivate, int iReason, void *pParam))
{
    ThreadScanFileData *pThreadData = calloc(1, sizeof(ThreadScanFileData));

    int rc = -1;

    do
    {
        if (pThreadData == NULL || pszFileName == NULL)
            break;

        pThreadData->hLib = hLib;
        pThreadData->pszFileName = strdup(pszFileName);
        pThreadData->iDataType = iDataType;
        pThreadData->iAction = iAction;
        pThreadData->iCompressFlag = iCompressFlag;
        pThreadData->pPrivate = pPrivate;
        pThreadData->pfCallBack = pfCallBack;

        if (pThreadData->pszFileName == NULL)
            break;

        if (TCSScanAsyncCreateThread(TCSScanFileAsyncWorker, pThreadData) != 0)
            break;

        rc = 0;
    }
    while (0);

    if (rc == -1)
    {
        if (pThreadData != NULL)
        {
            free(pThreadData->pszFileName);
            free(pThreadData);
        }
    }

    return rc;
}

static PluginContext *LoadPlugin(void)
{
    PluginContext *pCtx = NULL;
    void *pTmp = dlopen(PLUGIN_PATH, RTLD_LAZY);
    DDBG("%s", "load plugin\n");
    if (pTmp != NULL)
    {
        FuncLibraryOpen TmpLibraryOpen;
        FuncLibraryClose TmpLibraryClose;
        FuncGetLastError TmpGetLastError;
        FuncScanData TmpScanData;
        FuncScanFile TmpScanFile;
        FuncScanFileEx TmpScanFileEx;
        FuncGetVersion TmpGetVersion;
        FuncGetInfo TmpGetInfo;
        
        do
        {
            TmpLibraryOpen = dlsym(pTmp, "TCSPLibraryOpen");
            DDBG("%s", "load api TCSPLibraryOpen\n");
            if (TmpLibraryOpen == NULL)
            {
                DDBG("Failed to load TCSPLibraryOpen in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpLibraryClose = dlsym(pTmp, "TCSPLibraryClose");
            if (TmpLibraryClose == NULL)
            {
                DDBG("Failed to load TCSPLibraryClose in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpGetLastError = dlsym(pTmp, "TCSPGetLastError");
            if (TmpGetLastError == NULL)
            {
                DDBG("Failed to load TCSPGetLastError in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpScanData = dlsym(pTmp, "TCSPScanData");
            if (TmpScanData == NULL)
            {
                DDBG("Failed to load TCSPScanData in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpScanFile = dlsym(pTmp, "TCSPScanFile");
            if (TmpScanFile == NULL)
            {
                DDBG("Failed to load TCSPScanFile in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpScanFileEx = dlsym(pTmp, "TCSPScanFileEx");
            if (TmpScanFileEx == NULL)
            {
                DDBG("Failed to load TCSPScanFileEx in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }
            
            TmpGetVersion = dlsym(pTmp, "TCSPGetVersion");
            if(TmpGetVersion == NULL)
            {
                DDBG("Failed to load TCSPGetVersion in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            TmpGetInfo = dlsym(pTmp, "TCSPGetInfo");
            if(TmpGetInfo == NULL)
            {
                DDBG("Failed to load TCSPGetInfo in %s\n", PLUGIN_PATH);
                dlclose(pTmp);
                break;
            }

            pCtx = (PluginContext *) malloc(sizeof(PluginContext));
            if (pCtx == NULL)
            {
                dlclose(pTmp);
                break;
            }
            pCtx->pPlugin = pTmp;
            pCtx->pfLibraryOpen = TmpLibraryOpen;
            pCtx->pfLibraryClose = TmpLibraryClose;
            pCtx->pfGetLastError = TmpGetLastError;
            pCtx->pfScanData = TmpScanData;
            pCtx->pfScanFile = TmpScanFile;
            pCtx->pfScanFileEx = TmpScanFileEx;
            pCtx->pfGetVersion = TmpGetVersion;
            pCtx->pfGetInfo = TmpGetInfo;

        } while(0);
    }
    else
    {
        DDBG("No plugin found. %s\n", dlerror());
    }

    return pCtx;
}


