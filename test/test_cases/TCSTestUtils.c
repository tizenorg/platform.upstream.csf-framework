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
#include "TCSErrorCodes.h"
#include "TCSImpl.h"
#include "TCSTest.h"

/* Concurrency test macros. */
#define CONTEST_START \
{\
    int iTestRet = 1;

#define CONTEST_ERROR \
    CONTEST_ASSERT(0)

#define CONTEST_ASSERT(condition) \
if (!(condition)) \
{ \
    LOG_OUT("test failed: %s,%d\n", __FILE__, __LINE__); \
    iTestRet = -1; \
}

#define CONTEST_RETURN(ret) \
    ret = iTestRet; \
}

#define CONTEST_RELEASE(con_test_ctx) \
    ReleaseTestObject(con_test_ctx, iTestRet);\
}

#define CONTESTREPLY_RELEASE(con_test_ctx) \
    ReleaseTestObject(con_test_ctx, iTestRet);\

#define TCS_ACTION_CLASS(a) ((a) & 0xff)
#define TCS_ACTION_BEHAVIOR(a) (((a) >> 8) & 0xff)

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define CONTENTS_ROOT "contents"
#define CONTENTS_TMP "tmp"


/**
 * Scan context
 */
typedef struct ScanContext_struct
{
    /* Test status */
    TestCase *pCurrentTestCase;
    char *pData;
    unsigned int uSize;
} ScanContext;


/**
 * Concurrency Scan context
 */
typedef struct ConScanContext_struct
{
    /* Test status */
    int iTestType;
    int iInfected;
    int iPolarity;
    int iTestRet;
    int *pFlags;
    char *pData;
    unsigned int uSize;
    ConTestContext *pConCtx;
} ConScanContext;


pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_Cond = PTHREAD_COND_INITIALIZER;


static int ScanBufferProc(TestCase *pCtx, char *pData, int iDataLen);
static int SaveTestContents(void);
static int LoadTestContents(void);
static char *GetBackupDir(void);
static void PutBackupDir(char *pszBackupDir);
static char *GetSamplePath(TestCase *pCtx);
static void PutSamplePath(char *pszSamplePath);
static int GetSampleDataType(int iTType);
static char *LoadFile(char const *pszFileName, int *piSize);
static void PutLoadedFile(char *pData);
static void ReportTestCase(TestCase *pCtx);
static TCSOffset CbScanGetSize(void *pPrivate);
static unsigned int CbScanRead(void *pPrivate, TCSOffset uOffset,
                               void *pBuffer, unsigned int uSize);
static unsigned int CbScanWrite(void *pPrivate, TCSOffset uOffset,
                                void const *pBuffer, unsigned int uSize);
static int CbScanSetSize(void *pPrivate, TCSOffset uSize);
static void CheckDetected(TestCase *pCtx, TCSDetected *pFound);
static void CheckDetectedList(TestCase *pCtx, TCSScanResult *pSR);
static int ScanBuffer(TestCase *pCtx);
static int ScanFile(TestCase *pCtx);
static char *GetTestRoot(void);
static void PutTestRoot(char *pszRoot);
static int VerifyRepairData(TestCase *pCtx, const char *pRepairedBuffer,
                            int iRepairedLen);
static int VerifyRepairFile(TestCase *pCtx);
static void PutBenignSamplePath(char *pszPath);
static char *GetBenignSamplePath(int iTType);
static int Infected(TestCase *pCtx, const char *pData, int iDataLen);
static int InfectedFile(TestCase *pCtx, const char *pszPath);
static void TestCaseCtorEx(TestCase *pCtx, const char *pszAPI, int iTestType,
                           int iPolarity, int iAction, int iCompressFlag,
                           PFScan pfCallback);
static void CallSys(const char *pszCmd);

/* Concurrency test functions. */
static char *ConGetSampleDir(int iCid);
static void ConPutSampleDir(char *pszDir);
static void ConCreateSampleDirs(void);
static void ConDestorySampleDirs(void);
static char *ConGetSamplePath(int iTType, int iPolarity, int iCid);
static void ConPutSamplePath(char *pszSamplePath);
static void ConTestCaseCtor(ConTestContext *pConCtx, int iCid, TestCase *pCtx);
static void ConTestCaseDtor(ConTestContext *pConCtx);

static void ConTestCaseCtorAsync(ConTestContext *pConCtx, TCSLIB_HANDLE hLib, int iCid, TestCase *pCtx);

static int ConWaitOnTestCond(ConTestContext *pConCtxAry);
static void ReleaseTestObject(ConTestContext *pConCtx, int iResult);

static int ConCheckDetected(int iTType, TCSDetected *pFound, int *pFlags);
static int ConCheckDetectedList(int iTType, TCSScanResult *pSR, int *pFlags);
static int ConVerifyRepairData(int iTType, int iCompressFlag, const char *pRepairedBuffer,
                               int iRepairedLen);
static int ConVerifyRepairFile(char *pszSamplePath, int iTType, int iCompressFlag);

static TCSOffset ConCbScanGetSize(void *pPrivate);
static unsigned int ConCbScanRead(void *pPrivate, TCSOffset uOffset,
                                  void *pBuffer, unsigned int uSize);
static unsigned int ConCbScanWrite(void *pPrivate, TCSOffset uOffset,
                                void const *pBuffer, unsigned int uSize);
static int ConCbScanSetSize(void *pPrivate, TCSOffset uSize);
static int ConCbScanCallback(void *pPrivate, int nReason, void *pParam);
static int ConCbScanCallbackAsync(void *pPrivate, int nReason, void *pParam);

static void ConScanDataProc(ConTestContext *pConCtx, int iAction);
static void *ConScanDataSCProc(void *pConCtxParam);
static void *ConScanDataSRProc(void *pConCtxParam);
static void *ConScanDataSCProcAsync(void *pConCtxParam);

static int ConTestSuccess(ConTestContext *pConCtxAry);
static int ConTestComplete(ConTestContext *pConCtxAry);

static void ConScanFileProc(ConTestContext *pConCtx, int iAction);
static void *ConScanFileSCProc(void *pConCtxParam);
static void *ConScanFileSRProc(void *pConCtxParam);
static void *ConScanFileSCProcAsync(void *pConCtxParam);

static int ConInfectedFile(int iType, int iCompressFlag, const char *pszPath);
static int ConInfected(int iType, int iCompressFlag, char *pData, int iDataLen);


int TestCasesCount = 0;
int Success = 0;
int Failures = 0;
jmp_buf JmpBuf; 


static char *LoadFile(char const *pszFileName, int *piSize)
{
    FILE *pFile = NULL;
    char *pData = NULL;

    if ((pFile = fopen(pszFileName, "rb")) == NULL)
        return NULL;

    fseek(pFile, 0, SEEK_END);
    *piSize = (long) ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    if ((pData = (char *) malloc(*piSize + 1)) == NULL)
    {
        fclose(pFile);
        return NULL;
    }
    if (fread(pData, 1, (size_t) *piSize, pFile) != *piSize)
    {
        free(pData);
        fclose(pFile);
        return NULL;
    }
    fclose(pFile);
    pData[*piSize] = '\0';

    return pData;
}


static void PutLoadedFile(char *pData)
{

    if (pData != NULL)
        free(pData);
}


/**
 * Output for test case result.
 */
static void ReportTestCase(TestCase *pCtx)
{
    char *pszTmp;

    LOG_OUT("@@@@@@@@@@@@@@@@@@@@@@@@\n");

    LOG_OUT("@ID: TC_SEC_CS_%s\n", pCtx->szAPIName);
    pszTmp = strchr(pCtx->szAPIName, '_');
    *pszTmp = 0;
    LOG_OUT("@API Name: %s\n", pCtx->szAPIName);
    *pszTmp = '_';
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static TCSOffset CbScanGetSize(void *pPrivate)
{
    ScanContext *pCtx = (ScanContext *) pPrivate;

    return pCtx->uSize;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static unsigned int CbScanRead(void *pPrivate, TCSOffset uOffset,
                               void *pBuffer, unsigned int uSize)
{
    unsigned int uRead = 0;
    ScanContext *pCtx = (ScanContext *) pPrivate;

    if (uOffset < pCtx->uSize)
    {
        if ((uRead = pCtx->uSize - uOffset) > uSize)
            uRead = uSize;
    }
    if (uRead)
        memcpy(pBuffer, pCtx->pData + uOffset, uRead);

    return uRead;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static unsigned int CbScanWrite(void *pPrivate, TCSOffset uOffset,
                                void const *pBuffer, unsigned int uSize)
{
    unsigned int uWrite = 0;
    ScanContext *pCtx = (ScanContext *) pPrivate;

    if (uOffset < pCtx->uSize)
    {
        if ((uWrite = pCtx->uSize - uOffset) > uSize)
            uWrite = uSize;
    }
    if (uWrite)
        memcpy(pCtx->pData + uOffset, pBuffer, uWrite);

    return uWrite;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static int CbScanSetSize(void *pPrivate, TCSOffset uSize)
{

    return 0;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
int CbScanCallback(void *pPrivate, int nReason, void *pParam)
{
    ScanContext *pCtx = (ScanContext *) pPrivate;
    const char *pszMalName = NULL, *pszVarName = NULL;
    int i, iTType = pCtx->pCurrentTestCase->iTestType;
    int n = SampleGetCount(iTType);

    TestCase TestCtx = *(pCtx->pCurrentTestCase);
    /*
     * Fix this is important since pParam could be different
     * if the nReason is not DETECTED.
     */
    if (nReason != TCS_CB_DETECTED)
        return 0;
    TEST_ASSERT(nReason == TCS_CB_DETECTED);
    TEST_ASSERT(pCtx->pCurrentTestCase->iPolarity == INFECTED_DATA);

    pCtx->pCurrentTestCase->iInfected++;

    for (i = 0; i < n; i++)
    {
        if (pCtx->pCurrentTestCase->pFlags[i])
            continue;

        pszMalName = SampleGetMalName(iTType, i);
        if (pszMalName != NULL)
        {
            TEST_ASSERT(((TCSDetected *) pParam)->pszName != NULL);
            if (((TCSDetected *) pParam)->pszName == NULL ||
                strcmp(((TCSDetected *) pParam)->pszName, pszMalName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        pszVarName = SampleGetVarName(iTType, i);
        if (pszVarName != NULL)
        {
            TEST_ASSERT(((TCSDetected *) pParam)->pszVariant != NULL);
            if (((TCSDetected *) pParam)->pszVariant == NULL ||
                strcmp(((TCSDetected *) pParam)->pszVariant, pszVarName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        TEST_ASSERT(TCS_ACTION_CLASS(((TCSDetected *) pParam)->uAction) ==
                    SampleGetSeverity(iTType, i));

        TEST_ASSERT(TCS_ACTION_BEHAVIOR(((TCSDetected *) pParam)->uAction) ==
                    SampleGetBehavior(iTType, i));

        pCtx->pCurrentTestCase->pFlags[i] = 1;
        break;
    }

    TEST_ASSERT(i != n);

    return 0;
}


/**
 * The difference between ScanFile and ScanBuffer is:
 * ScanBuffer is the helper function to test scan data.
 */
static int ScanBuffer(TestCase *pCtx)
{
    int iFSize;
    char *pData, *pszFilePath;

    if ((pszFilePath = GetSamplePath(pCtx)) == NULL)
        return -1;

    if ((pData = LoadFile(pszFilePath, &iFSize)) == NULL)
    {
        PutSamplePath(pszFilePath);
        return -1;
    }

    TestCase TestCtx = *pCtx;
    TEST_ASSERT(ScanBufferProc(pCtx, pData, iFSize) == 0);
    PutLoadedFile(pData);
    PutSamplePath(pszFilePath);

    return 0;
}


/**
 * Scan file test helper function.
 */
static int ScanFile(TestCase *pCtx)
{
    int iErr, iExpected = SampleGetCount(pCtx->iTestType);
    TCSScanResult SR;
    TCSLIB_HANDLE hLib;
    char *pszFilePath;

    TestCase TestCtx = *pCtx;
    if ((pszFilePath = GetSamplePath(pCtx)) == NULL)
        return -1;

    hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
    {
        PutSamplePath(pszFilePath);
        return -1;
    }

    if (iExpected > 0)
    {
        pCtx->pFlags = (int *) calloc(iExpected, sizeof(int));
        if (pCtx->pFlags == NULL)
            TEST_ASSERT(0);
    }
    else
        pCtx->pFlags = NULL;

    if (pCtx->pfCallback == CbAbortScanCallbackFileAsync)
    {
        TEST_ASSERT(TCSScanFileEx(hLib, pszFilePath,
                                GetSampleDataType(pCtx->iTestType), pCtx->iAction, pCtx->iCompressFlag,
                                NULL, pCtx->pfCallback, &SR) == -1);
        iErr = TCSGetLastError(hLib);
        TEST_ASSERT(TCS_ERRMODULE(iErr) == TCS_ERROR_MODULE_GENERIC);
        TEST_ASSERT(TCS_ERRCODE(iErr) == TCS_ERROR_CANCELLED);
    }
    else
    if (pCtx->iAction == TCS_SA_SCANREPAIR && IsTestRepair() == 0)
    {
        TEST_ASSERT(TCSScanFile(hLib, pszFilePath,
                                GetSampleDataType(pCtx->iTestType),
                                pCtx->iAction, pCtx->iCompressFlag, &SR) == -1);
        iErr = TCSGetLastError(hLib);
        TEST_ASSERT(TCS_ERRMODULE(iErr) == TCS_ERROR_MODULE_GENERIC);
        TEST_ASSERT(TCS_ERRCODE(iErr) == TCS_ERROR_NOT_IMPLEMENTED);
    }
    else
    {
        TEST_ASSERT(TCSScanFile(hLib, pszFilePath,
                                GetSampleDataType(pCtx->iTestType),
                                pCtx->iAction, pCtx->iCompressFlag, &SR) == 0);
        if (pCtx->pFlags)
            memset(pCtx->pFlags, 0, sizeof(int) * iExpected);


        if (pCtx->iAction == TCS_SA_SCANONLY)
        {
            if (pCtx->iPolarity == INFECTED_DATA)
            {
                if (pCtx->iTestType == MALWARE_TTYPE_COMPRESS &&
                    pCtx->iCompressFlag == 0)
                /* Not suppose to detect when compress flag is
                 * set to 0 for compressed samples. */
                {
                    TEST_ASSERT(SR.iNumDetected == 0);
                }
                else
                {
                    TEST_ASSERT(SR.iNumDetected == SampleGetCount(pCtx->iTestType));
                    if (SR.iNumDetected == SampleGetCount(pCtx->iTestType))
                        CheckDetectedList(pCtx, &SR);
                }
            }
            else
            {
                TEST_ASSERT(SR.iNumDetected == 0);
            }
        }
        else
        {
            /* Verify */
            TEST_ASSERT(VerifyRepairFile(pCtx) == 0);
        }
        if (*SR.pfFreeResult != NULL)
            (*SR.pfFreeResult)(&SR);
    }

    if (pCtx->pFlags)
        free(pCtx->pFlags);

    TCSLibraryClose(hLib);
    PutSamplePath(pszFilePath);

    return 0;
}


/**
 * Helper function for detected one malware.
 */
static void CheckDetected(TestCase *pCtx, TCSDetected *pFound)
{
    const char *pszMalName = NULL, *pszVarName = NULL;
    int i, iTType = pCtx->iTestType, n = SampleGetCount(iTType);

    TestCase TestCtx = *pCtx;
    for (i = 0; i < n; i++)
    {
        if (pCtx->pFlags[i])
            continue;

        pszMalName = SampleGetMalName(iTType, i);
        if (pszMalName != NULL)
        {
            TEST_ASSERT(pFound->pszName != NULL);
            if (pFound->pszName == NULL ||
                strcmp(pFound->pszName, pszMalName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        pszVarName = SampleGetVarName(iTType, i);
        if (pszVarName != NULL)
        {
            TEST_ASSERT(pFound->pszVariant != NULL);
            if (pFound->pszVariant == NULL ||
                strcmp(pFound->pszVariant, pszVarName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        TEST_ASSERT(TCS_ACTION_CLASS(pFound->uAction) ==
                    SampleGetSeverity(iTType, i));

        TEST_ASSERT(TCS_ACTION_BEHAVIOR(pFound->uAction) ==
                    SampleGetBehavior(iTType, i));

        pCtx->pFlags[i] = 1;
        break;
    }

    TEST_ASSERT(i != n);
}


/**
 * Helper function for detected malware list checking.
 */
static void CheckDetectedList(TestCase *pCtx, TCSScanResult *pSR)
{
    TCSDetected *pFound = pSR->pDList;

    while (pFound != NULL)
    {
        CheckDetected(pCtx, pFound);
        pFound = pFound->pNext;
    }
}


/**
 * Helper function for data scan, see ScanBuffer()
 */
static int ScanBufferProc(TestCase *pCtx, char *pData, int iDataLen)
{
    TCSLIB_HANDLE hLib;
    TCSScanParam SP = {0};
    TCSScanResult SR = {0};
    ScanContext ScanCtx = {0};
    int iErr, iExpected = SampleGetCount(pCtx->iTestType);
    TestCase TestCtx = *pCtx;

    hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
        return -1;

    ScanCtx.pData = pData;
    ScanCtx.uSize = (unsigned int) iDataLen;
    ScanCtx.pCurrentTestCase = pCtx;

    if (iExpected > 0)
    {
        pCtx->pFlags = (int *) calloc(iExpected, sizeof(int));
        if (pCtx->pFlags == NULL)
            TEST_ASSERT(0);
    }
    else
        pCtx->pFlags = NULL;

    SP.iAction = pCtx->iAction;
    SP.iDataType = GetSampleDataType(pCtx->iTestType);
    SP.iCompressFlag = pCtx->iCompressFlag;
    SP.pPrivate = &ScanCtx;
    SP.pfGetSize = CbScanGetSize;
    SP.pfSetSize = CbScanSetSize;
    SP.pfRead = CbScanRead;
    SP.pfWrite = CbScanWrite;
    SP.pfCallBack = pCtx->pfCallback;

    if (pCtx->pfCallback == &CbAbortCallback)
    {
        TEST_ASSERT(TCSScanData(hLib, &SP, &SR) == -1);
        iErr = TCSGetLastError(hLib);
        LOG_OUT("ScanBufferProc TCSErrormodule = %d, TCSErrorCode = %d\n",
                TCS_ERRMODULE(iErr), TCS_ERRCODE(iErr));
        TEST_ASSERT(TCS_ERRMODULE(iErr) == TCS_ERROR_MODULE_GENERIC);
        TEST_ASSERT(TCS_ERRCODE(iErr) == TCS_ERROR_CANCELLED);
    }
    else if (pCtx->iAction == TCS_SA_SCANREPAIR && IsTestRepair() == 0)
    {
        TEST_ASSERT(TCSScanData(hLib, &SP, &SR) == -1);
        iErr = TCSGetLastError(hLib);
        TEST_ASSERT(TCS_ERRMODULE(iErr) == TCS_ERROR_MODULE_GENERIC);
        TEST_ASSERT(TCS_ERRCODE(iErr) == TCS_ERROR_NOT_IMPLEMENTED);
    }
    else
    {
        TEST_ASSERT(TCSScanData(hLib, &SP, &SR) == 0);
        if (pCtx->pFlags)
            memset(pCtx->pFlags, 0, sizeof(int) * iExpected);

        /* Make sure pfCallback is called as expected. */
        if (pCtx->pfCallback == &CbScanCallback &&
            pCtx->iAction == TCS_SA_SCANONLY &&
            pCtx->iPolarity == INFECTED_DATA)
        {
            if (pCtx->iTestType == MALWARE_TTYPE_COMPRESS &&
                pCtx->iCompressFlag == 0)
            /* Not suppose to detect virus when compress flag is
             * disabled for compress test sample */
            {
                TEST_ASSERT(SR.iNumDetected == 0);
            }
            else
            {
                TEST_ASSERT(pCtx->iInfected == iExpected);
            }
        }

        if (pCtx->iAction == TCS_SA_SCANONLY)
        {
            if (pCtx->iPolarity == INFECTED_DATA)
            {
                if (pCtx->iTestType == MALWARE_TTYPE_COMPRESS &&
                    pCtx->iCompressFlag == 0)
                /* Not suppose to detect virus when compress flag is
                 * disabled for compress test sample */
                {
                    TEST_ASSERT(SR.iNumDetected == 0);
                }
                else
                {
                    TEST_ASSERT(SR.iNumDetected == iExpected);
                    if (SR.iNumDetected == iExpected)
                        CheckDetectedList(pCtx, &SR);
                }
            }
            else
            {
                TEST_ASSERT(SR.iNumDetected == 0);
            }
        }
        else
        {
            TEST_ASSERT(VerifyRepairData(pCtx, ScanCtx.pData,
                                         ScanCtx.uSize) == 0);
        }

        if (*SR.pfFreeResult)
            (*SR.pfFreeResult)(&SR);
    }

    if (pCtx->pFlags)
        free(pCtx->pFlags);

    TCSLibraryClose(hLib);

    return 0;
}


/**
 * Abort test callback helper function for data scan.
 */
int CbAbortCallback(void *pPrivate, int nReason, void *pParam)
{

    return -1;
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

                pszEnv = getenv("TCS_CONTENT_PATH");
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
    char *pszCommand, *pszEnv = getenv("TCS_CONTENT_PATH");

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


/**
 * Test framework helper function: get content files' root path.
 */
static char *GetTestRoot(void)
{
    int iLen, iEnvLen;
    char *pszRoot = NULL, *pszEnv = getenv("TCS_CONTENT_PATH");

    if (pszEnv != NULL &&
        (iEnvLen = strlen(pszEnv)) > 0)
    {
        iLen = iEnvLen;
        iLen += 64; /* Reserved 64 bytes for PID. */
        iLen += strlen(TCS_TEST_CONTENT_DIR);
        pszRoot = (char *) calloc(iLen + 1, sizeof(char));
        if (pszRoot != NULL)
        {
            if (pszEnv[iEnvLen - 1] != '/')
                snprintf(pszRoot, iLen, "%s/%d/%s", pszEnv, (int) getpid(),
                         TCS_TEST_CONTENT_DIR);
            else
                snprintf(pszRoot, iLen, "%s%d/%s", pszEnv, (int) getpid(),
                         TCS_TEST_CONTENT_DIR);
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


void TestCaseCtor(TestCase *pCtx, const char *pszAPI, int iTestType,
                  int iPolarity, int iAction, PFScan pfCallback)
{

    TestCaseCtorEx(pCtx, pszAPI, iTestType, iPolarity, iAction, 1, pfCallback);
}


/**
 * Test case constructor.
 */
static void TestCaseCtorEx(TestCase *pCtx, const char *pszAPI, int iTestType,
                           int iPolarity, int iAction, int iCompressFlag,
                           PFScan pfCallback)
{
    TestCase TestCtx = *pCtx;
    strncpy(pCtx->szAPIName, pszAPI, sizeof(pCtx->szAPIName) - 1);
    pCtx->iInfected = 0;
    pCtx->iTestType = iTestType;
    pCtx->iPolarity = iPolarity;
    pCtx->iAction = iAction;
    pCtx->iCompressFlag = iCompressFlag;
    pCtx->pfCallback = pfCallback;
    pCtx->pfCallback = NULL;
    pCtx->pFlags = NULL;
    TEST_ASSERT(SaveTestContents() == 0);
}


/**
 * Test case destructor.
 */
void TestCaseDtor(TestCase *pCtx)
{
    TestCase TestCtx = *pCtx;
    ReportTestCase(pCtx);
    TEST_ASSERT(LoadTestContents() == 0);
    Success++;
}


static char *GetBackupDir(void)
{
    int iLen, iEnvLen;
    char *pszEnv = getenv("TCS_CONTENT_PATH"), *pszPath;

    if (pszEnv == NULL || strlen(pszEnv) == 0)
        pszEnv = "./";
    iEnvLen = strlen(pszEnv);
    iLen = iEnvLen;
    iLen += strlen(TCS_BACKUP_CONTENT_DIR);
    iLen += 64; /* Reserved for slash char and PID. */
    pszPath = (char *) calloc(iLen + 1, sizeof(char));

    if (pszPath)
    {
        if (pszEnv[iEnvLen - 1] == '/')
            snprintf(pszPath, iLen, "%s%d/%s", pszEnv, (int) getpid(),
                     TCS_BACKUP_CONTENT_DIR);
        else
            snprintf(pszPath, iLen, "%s/%d/%s", pszEnv, (int) getpid(),
                     TCS_BACKUP_CONTENT_DIR);
    }

    return pszPath;
}


static void PutBackupDir(char *pszBackupDir)
{

    if (pszBackupDir != NULL)
        free(pszBackupDir);
}


static int SaveTestContents(void)
{
    int iLen = 0, iRet = -1;
    char *pszRoot = NULL, *pszCommand = NULL, *pszBackupDir = GetBackupDir();

    if (pszBackupDir != NULL)
    {
        pszRoot = GetTestRoot();
        if (pszRoot != NULL)
        {
            iLen = strlen(pszBackupDir);
            iLen += strlen(pszRoot);
            iLen += 32; /* Reserved for unix commmand. */

            pszCommand = (char *) calloc(iLen, sizeof(char));
            if (pszCommand != NULL)
            {
                snprintf(pszCommand, iLen - sizeof(char), "cp -f %s/* %s/ 2>/dev/null",
                         pszRoot, pszBackupDir);

                CallSys(pszCommand);
                free(pszCommand);
                iRet = 0;
            }

            PutTestRoot(pszRoot);
        }

        PutBackupDir(pszBackupDir);
    }

    return iRet;
}


static int LoadTestContents(void)
{
    int iLen = 0, iRet = -1;
    char *pszRoot = NULL, *pszCommand = NULL, *pszBackupDir = GetBackupDir();

    if (pszBackupDir != NULL)
    {
        pszRoot = GetTestRoot();
        if (pszRoot != NULL)
        {
            iLen = strlen(pszBackupDir);
            iLen += strlen(pszRoot);
            iLen += 32; /* Reserved for unix command. */

            pszCommand = (char *) calloc(iLen, sizeof(char));
            if (pszCommand != NULL)
            {
                snprintf(pszCommand, iLen - sizeof(char), "cp -f %s/* %s/ 2>/dev/null",
                         pszBackupDir, pszRoot);

                CallSys(pszCommand);
                free(pszCommand);
                iRet = 0;
            }
            PutTestRoot(pszRoot);
        }

        PutBackupDir(pszBackupDir);
    }

    return iRet;
}


static char *GetSamplePath(TestCase *pCtx)
{
    const char *pszSampleFileName;
    char *pszSamplePath = NULL, *pszRoot = GetTestRoot();
    int iLen, iTType = pCtx->iTestType, iPolarity = pCtx->iPolarity;
    char *pwd = getenv("PWD");

    if (pszRoot != NULL)
    {
        pszSampleFileName = (iPolarity == INFECTED_DATA ?
                             SampleGetInfectedFileName(iTType) :
                             SampleGetBenignFileName(iTType));

        iLen = strlen(pszRoot);
        iLen += strlen(pszSampleFileName);
        iLen++; /* Reserved for slash char. */
        iLen += strlen(pwd) + 3;

        pszSamplePath = (char *) calloc(iLen + 1, sizeof(char));

        if (pszSamplePath != NULL)
        {
            if (pszRoot[strlen(pszRoot) - 1] == '/')
                snprintf(pszSamplePath, iLen, "%s/%s%s", pwd, pszRoot, pszSampleFileName);
            else
                snprintf(pszSamplePath, iLen, "%s/%s/%s", pwd, pszRoot, pszSampleFileName);
        }

        PutTestRoot(pszRoot);
    }

    return pszSamplePath;
}


static void PutSamplePath(char *pszSamplePath)
{

    if (pszSamplePath != NULL)
        free(pszSamplePath);
}


static int GetSampleDataType(int iTType)
{
    switch (iTType)
    {
    case MALWARE_TTYPE_BUFFER:
        return TCS_DTYPE_UNKNOWN;
    case MALWARE_TTYPE_HTML:
        return TCS_DTYPE_HTML;
    case MALWARE_TTYPE_URL:
        return TCS_DTYPE_URL;
    case MALWARE_TTYPE_EMAIL:
        return TCS_DTYPE_EMAIL;
    case MALWARE_TTYPE_PHONE:
        return TCS_DTYPE_PHONE;
    case MALWARE_TTYPE_TEXT:
        return TCS_DTYPE_TEXT;
    case MALWARE_TTYPE_JAVA:
        return TCS_DTYPE_JAVA;
    case MALWARE_TTYPE_JAVAS:
        return TCS_DTYPE_JAVAS;
    case MALWARE_TTYPE_MULTIPLE:
        return TCS_DTYPE_UNKNOWN;
    case MALWARE_TTYPE_COMPRESS:
        return TCS_DTYPE_UNKNOWN;
    case MALWARE_TTYPE_MULTIPLE_BYTES:
        return TCS_DTYPE_TEXT;
    default:
        return 0; /* Unlikely be here. */
    }
}


void TestScanData(const char *pszFunc, int iTType, int iPolarity,
                  int iAction, PFScan pfCallback)
{
    TestCase TestCtx;

    TESTCASECTOREX(&TestCtx, pszFunc, iTType, iPolarity, iAction, 1, pfCallback);
    ScanBuffer(&TestCtx);
    TESTCASEDTOR(&TestCtx);
}


void TestScanFile(const char *pszFunc, int iTType, int iPolarity, int iAction)
{
    TestCase TestCtx;

    TESTCASECTOREX(&TestCtx, pszFunc, iTType, iPolarity, iAction, 1, NULL);
    TEST_ASSERT(ScanFile(&TestCtx) == 0);
    TESTCASEDTOR(&TestCtx);
}

void TestScanFileExCancel(const char *pszFunc, int iTType, int iPolarity, int iAction)
{
    TestCase TestCtx;

    TESTCASECTOREX(&TestCtx, pszFunc, iTType, iPolarity, iAction, 1, NULL);
    TestCtx.pfCallback = CbAbortScanCallbackFileAsync;
    TEST_ASSERT(ScanFile(&TestCtx) == 0);
    TESTCASEDTOR(&TestCtx);
}

void TestScanDataEx(const char *pszFunc, int iTType, int iPolarity,
                    int iAction, int iCompressFlag, PFScan pfCallback)
{
    TestCase TestCtx;

    TESTCASECTOREX(&TestCtx, pszFunc, iTType, iPolarity, iAction,
                   iCompressFlag, pfCallback);
    TEST_ASSERT(ScanBuffer(&TestCtx) == 0);
    TESTCASEDTOR(&TestCtx);
}


void TestScanFileEx(const char *pszFunc, int iTType, int iPolarity,
                    int iAction, int iCompressFlag)
{
    TestCase TestCtx;

    TESTCASECTOREX(&TestCtx, pszFunc, iTType, iPolarity,
                   iAction, iCompressFlag, NULL);
    TEST_ASSERT(ScanFile(&TestCtx) == 0);
    TESTCASEDTOR(&TestCtx);
}


static int ConInfectedFile(int iType, int iCompressFlag, const char *pszPath)
{
    int iRet = -1;
    TCSScanResult SR = {0};
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;

    hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
        return -1;
    iRet = TCSScanFile(hLib, pszPath, GetSampleDataType(iType),
                       TCS_SA_SCANONLY, iCompressFlag, &SR);
    if (iRet != 0)
    {
        TCSLibraryClose(hLib);
        return -1;
    }
    iRet = SR.iNumDetected;
    if (SR.pfFreeResult != NULL)
        SR.pfFreeResult(&SR);
    TCSLibraryClose(hLib);

    return iRet;
}


static int ConInfected(int iType, int iCompressFlag, char *pData, int iDataLen)
{
    int iRet = -1;
    TCSScanParam SP = {0};
    TCSScanResult SR = {0};
    ScanContext ScanCtx = {0};
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;

    ScanCtx.pData = pData;
    ScanCtx.uSize = (unsigned int) iDataLen;

    SP.iAction = TCS_SA_SCANONLY;
    SP.iDataType = GetSampleDataType(iType);
    SP.iCompressFlag = iCompressFlag;
    SP.pPrivate = &ScanCtx;
    SP.pfGetSize = CbScanGetSize;
    SP.pfSetSize = CbScanSetSize;
    SP.pfRead = CbScanRead;
    SP.pfWrite = CbScanWrite;
    SP.pfCallBack = NULL;

    hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
        return -1;
    iRet = TCSScanData(hLib, &SP, &SR);
    if (iRet != 0)
    {
        TCSLibraryClose(hLib);
        return -1;
    }
    iRet = SR.iNumDetected;
    if (SR.pfFreeResult != NULL)
        SR.pfFreeResult(&SR);
    TCSLibraryClose(hLib);

    return iRet;
}


static int InfectedFile(TestCase *pCtx, const char *pszPath)
{
    int iRet = -1;
    TCSScanResult SR = {0};
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx = *pCtx;

    hLib = TCSLibraryOpen();
    TEST_ASSERT(hLib != INVALID_TCSLIB_HANDLE);
    TEST_ASSERT(TCSScanFile(hLib, pszPath,
                            GetSampleDataType(pCtx->iTestType),
                            TCS_SA_SCANONLY, pCtx->iCompressFlag, &SR) == 0);
    iRet = SR.iNumDetected;
    if (SR.pfFreeResult != NULL)
        SR.pfFreeResult(&SR);
    TCSLibraryClose(hLib);

    return iRet;
}


static int Infected(TestCase *pCtx, const char *pData, int iDataLen)
{
    int iRet = -1;
    TCSScanParam SP = {0};
    TCSScanResult SR = {0};
    ScanContext ScanCtx = {0};
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx = *pCtx;

    ScanCtx.pData = (char *) pData;
    ScanCtx.uSize = (unsigned int) iDataLen;
    ScanCtx.pCurrentTestCase = pCtx;

    SP.iAction = TCS_SA_SCANONLY;
    SP.iDataType = GetSampleDataType(pCtx->iTestType);
    SP.iCompressFlag = pCtx->iCompressFlag;
    SP.pPrivate = &ScanCtx;
    SP.pfGetSize = CbScanGetSize;
    SP.pfSetSize = CbScanSetSize;
    SP.pfRead = CbScanRead;
    SP.pfWrite = CbScanWrite;
    SP.pfCallBack = NULL;

    hLib = TCSLibraryOpen();
    TEST_ASSERT(hLib != INVALID_TCSLIB_HANDLE);
    TEST_ASSERT(TCSScanData(hLib, &SP, &SR) == 0);
    iRet = SR.iNumDetected;
    if (SR.pfFreeResult != NULL)
        SR.pfFreeResult(&SR);
    TCSLibraryClose(hLib);

    return iRet;
}


static int VerifyRepairData(TestCase *pCtx, const char *pRepairedBuffer,
                            int iRepairedLen)
{

    return Infected(pCtx, pRepairedBuffer, iRepairedLen);
}


static int VerifyRepairFile(TestCase *pCtx)
{
    char *szSamplePath;
    int iRet = -1;
    szSamplePath = GetSamplePath(pCtx);
    iRet = InfectedFile(pCtx, szSamplePath);
    PutSamplePath(szSamplePath);
    return iRet;
}


static void ConTestCaseCtor(ConTestContext *pConCtx, int iCid, TestCase *pCtx)
{

    pConCtx->pTestCtx = pCtx;
    pConCtx->iCid = iCid;
    pConCtx->iConTestRet = 0; /* running. */
}

static void ConTestCaseCtorAsync(ConTestContext *pConCtx, TCSLIB_HANDLE hLib,
                                 int iCid, TestCase *pCtx)
{
    pConCtx->pTestCtx = pCtx;
    pConCtx->iCid = iCid;
    pConCtx->iConTestRet = 0; /* running. */
    pConCtx->pTestCtx->hLib = hLib;
}

static void ConTestCaseDtor(ConTestContext *pConCtx)
{

}


static char *GetBenignSamplePath(int iTType)
{
    int iLen, iRootLen;
    char *pszSamplePath = NULL, *pszRoot = GetTestRoot();
    const char *pszSampleFileName = NULL;

    if (pszRoot != NULL)
    {
        pszSampleFileName = SampleGetBenignFileName(iTType);
        iRootLen = strlen(pszRoot);
        iLen = iRootLen;
        iLen += strlen(pszSampleFileName);
        iLen += 2; /* Slash char and \0 */

        pszSamplePath = (char *) calloc(iLen + 1, sizeof(char));
        if (pszSamplePath != NULL)
        {
            if (pszRoot[iRootLen - 1] == '/')
                snprintf(pszSamplePath, iLen, "%s%s", pszRoot, pszSampleFileName);
            else
                snprintf(pszSamplePath, iLen, "%s/%s", pszRoot, pszSampleFileName);
        }
        PutTestRoot(pszRoot);
    }

    return pszSamplePath;
}


static void PutBenignSamplePath(char *pszPath)
{
    if (pszPath != NULL)
        free(pszPath);
}


static int ConVerifyRepairData(int iTType, int iCompressFlag, const char *pRepairedBuffer,
                               int iRepairedLen)
{

    return ConInfected(iTType, iCompressFlag, (char *) pRepairedBuffer, iRepairedLen);
}


static int ConVerifyRepairFile(char *pszSamplePath, int iTType, int iCompressFlag)
{

    return ConInfectedFile(iTType, iCompressFlag, pszSamplePath);
}


static int ConTestComplete(ConTestContext *pConCtxAry)
{
    int i;

    for (i = 0; i < MAX_TEST_THREADS; i++)
    {
        if (pConCtxAry[i].iConTestRet == 0)
            return 0; /* not complete */
    }

    return 1; /* Complete */
}


static int ConTestSuccess(ConTestContext *pConCtxAry)
{
    int i;

    for (i = 0; i < MAX_TEST_THREADS; i++)
    {
        if (pConCtxAry[i].iConTestRet != 1)
            return 0; /* failure */
    }

    return 1; /* success */
}


static void ReleaseTestObject(ConTestContext *pConCtx, int iResult)
{

    pthread_mutex_lock(&g_Mutex);
    pConCtx->iConTestRet = iResult;
    pthread_cond_broadcast(&g_Cond);
    pthread_mutex_unlock(&g_Mutex);
}


static int ConCheckDetected(int iTType, TCSDetected *pFound, int *pFlags)
{
    const char *pszMalName = NULL, *pszVarName = NULL;
    int i, n = SampleGetCount(iTType), iRet = 0;

    for (i = 0; i < n; i++)
    {
        if (pFlags[i])
            continue;

        pszMalName = SampleGetMalName(iTType, i);
        if (pszMalName != NULL)
        {
            if (pFound->pszName == NULL ||
                strcmp(pFound->pszName, pszMalName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        pszVarName = SampleGetVarName(iTType, i);
        if (pszVarName != NULL)
        {
            if (pFound->pszVariant == NULL ||
                strcmp(pFound->pszVariant, pszVarName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        if (TCS_ACTION_CLASS(pFound->uAction) != SampleGetSeverity(iTType, i))
            iRet = -1;

        if (TCS_ACTION_BEHAVIOR(pFound->uAction) != SampleGetBehavior(iTType, i))
            iRet = -1;

        pFlags[i] = 1;
        break;
    }

    if (i >= n)
        iRet = -1;

    return iRet;
}


static int ConCheckDetectedList(int iTType, TCSScanResult *pSR, int *pFlags)
{
    int iRet = 0;
    TCSDetected *pFound = pSR->pDList;

    while (pFound != NULL)
    {
        iRet = ConCheckDetected(iTType, pFound, pFlags);
        if (iRet == -1)
            break;
        pFound = pFound->pNext;
    }

    return iRet;
}


static void *ConScanDataSCProc(void *pConCtxParam)
{
    ConTestContext *pConCtx = (ConTestContext *) pConCtxParam;

    ConScanDataProc(pConCtx, TCS_SA_SCANONLY);

    return NULL;
}


static void *ConScanDataSRProc(void *pConCtxParam)
{
    ConTestContext *pConCtx = (ConTestContext *) pConCtxParam;

    ConScanDataProc(pConCtx, TCS_SA_SCANREPAIR);

    return NULL;
}


static void ConScanDataProc(ConTestContext *pConCtx, int iAction)
{
    TCSLIB_HANDLE hLib;
    TCSScanParam SP;
    TCSScanResult SR = {0};
    ConScanContext ScanCtx = {0};
    char *pszSamplePath = NULL, *pData;
    int i, j, iDataLen = 0, iCid= pConCtx->iCid, iOldType, iExpected;
    int *pFlags;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    i = MALWARE_TTYPE_BUFFER;
    for (i = MALWARE_TTYPE_BUFFER; i < MALWARE_TTYPE_MULTIPLE; i++)
    {
        if ((iExpected = SampleGetCount(i)) > 0)
        {
            pFlags = (int *) malloc(sizeof(int) * iExpected);
            if (pFlags == NULL)
                CONTEST_ERROR
        }
        else
            pFlags = NULL;

        for (j = BENIGN_DATA; j <= INFECTED_DATA; j++)
        {
            hLib = TCSLibraryOpen();
            if (hLib != INVALID_TCSLIB_HANDLE)
            {
                /* Test benign data. */
                pszSamplePath = ConGetSamplePath(i, j, iCid);
                pData = LoadFile(pszSamplePath, &iDataLen);

                ConPutSamplePath(pszSamplePath);
                ScanCtx.pData = pData;
                ScanCtx.uSize = (unsigned int) iDataLen;
                ScanCtx.iTestType = i;
                ScanCtx.iInfected = 0;
                ScanCtx.iPolarity = j;
                ScanCtx.iTestRet = 1;
                ScanCtx.pFlags = pFlags;

                SP.iAction = iAction;
                SP.iDataType = GetSampleDataType(i);
                SP.iCompressFlag = 1;
                SP.pPrivate = &ScanCtx;
                SP.pfGetSize = ConCbScanGetSize;
                SP.pfSetSize = ConCbScanSetSize;
                SP.pfRead = ConCbScanRead;
                SP.pfWrite = ConCbScanWrite;
                SP.pfCallBack = ConCbScanCallback;

                if (pFlags)
                    memset(pFlags, 0, sizeof(int) * iExpected);

                if (TCSScanData(hLib, &SP, &SR) == 0)
                {
                    if (pFlags)
                        memset(pFlags, 0, sizeof(int) * iExpected);

                    if (j == BENIGN_DATA)
                    {
                        CONTEST_ASSERT(SR.iNumDetected == 0)
                    }
                    else
                    {
                        if (iAction == TCS_SA_SCANONLY)
                        {
                            CONTEST_ASSERT(SR.iNumDetected == iExpected &&
                                           ConCheckDetectedList(i, &SR, pFlags) == 0)
                        }
                        else /* Repair */
                        {
                            CONTEST_ASSERT(ConVerifyRepairData(i, pConCtx->pTestCtx->iCompressFlag,
                                                               ScanCtx.pData, ScanCtx.uSize) == 0)
                        }
                    }

                    CONTEST_ASSERT(ScanCtx.iTestRet == 1)
                    (*SR.pfFreeResult)(&SR);
                }
                else
                {
                    CONTEST_ERROR
                }

                PutLoadedFile(pData);

                TCSLibraryClose(hLib);
            }
            else
            {
                CONTEST_ERROR
            }

            pthread_testcancel();
            usleep(SLEEP_INTERVAL);
            pthread_testcancel();
        }

        if (pFlags)
            free(pFlags);
    }
    CONTEST_RELEASE(pConCtx);
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static TCSOffset ConCbScanGetSize(void *pPrivate)
{
    ConScanContext *pCtx = (ConScanContext *) pPrivate;

    return pCtx->uSize;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static unsigned int ConCbScanRead(void *pPrivate, TCSOffset uOffset,
                               void *pBuffer, unsigned int uSize)
{
    unsigned int uRead = 0;
    ConScanContext *pCtx = (ConScanContext *) pPrivate;

    if (uOffset < pCtx->uSize)
    {
        if ((uRead = pCtx->uSize - uOffset) > uSize)
            uRead = uSize;
    }
    if (uRead)
        memcpy(pBuffer, pCtx->pData + uOffset, uRead);

    return uRead;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static unsigned int ConCbScanWrite(void *pPrivate, TCSOffset uOffset,
                                   void const *pBuffer, unsigned int uSize)
{
    unsigned int uWrite = 0;
    ConScanContext *pCtx = (ConScanContext *) pPrivate;

    if (uOffset < pCtx->uSize)
    {
        if ((uWrite = pCtx->uSize - uOffset) > uSize)
            uWrite = uSize;
    }
    if (uWrite)
        memcpy(pCtx->pData + uOffset, pBuffer, uWrite);

    return uWrite;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static int ConCbScanSetSize(void *pPrivate, TCSOffset uSize)
{

    return 0;
}


/**
 * Callback helper for data scan, please reference API
 * specification for more information about data scan.
 */
static int ConCbScanCallback(void *pPrivate, int nReason, void *pParam)
{
    ConScanContext *pCtx = (ConScanContext *) pPrivate;
    const char *pszMalName = NULL, *pszVarName = NULL;
    int i, iTType = pCtx->iTestType, n = SampleGetCount(iTType);

    CONTEST_START

    /*
     * Fix this is important since pParam could be different
     * if the nReason is not DETECTED.
     */
    if (nReason != TCS_CB_DETECTED)
        return 0;

    CONTEST_ASSERT(nReason == TCS_CB_DETECTED)
    CONTEST_ASSERT(pCtx->iPolarity == INFECTED_DATA)

    pCtx->iInfected++;

    for (i = 0; i < n; i++)
    {
        if (pCtx->pFlags[i])
            continue;

        pszMalName = SampleGetMalName(iTType, i);
        if (pszMalName != NULL)
        {
            CONTEST_ASSERT(pParam != NULL);
            CONTEST_ASSERT(((TCSDetected *) pParam)->pszName != NULL);
            if (((TCSDetected *) pParam)->pszName == NULL ||
                strcmp(((TCSDetected *) pParam)->pszName, pszMalName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        pszVarName = SampleGetVarName(iTType, i);
        if (pszVarName != NULL)
        {
            CONTEST_ASSERT(((TCSDetected *) pParam)->pszVariant != NULL);
            if (((TCSDetected *) pParam)->pszVariant == NULL ||
                strcmp(((TCSDetected *) pParam)->pszVariant, pszVarName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        CONTEST_ASSERT(TCS_ACTION_CLASS(((TCSDetected *) pParam)->uAction) ==
                       SampleGetSeverity(iTType, i));

        CONTEST_ASSERT(TCS_ACTION_BEHAVIOR(((TCSDetected *) pParam)->uAction) ==
                       SampleGetBehavior(iTType, i));

        pCtx->pFlags[i] = 1;
        break;
    }

    CONTEST_ASSERT(i != n);

    CONTEST_RETURN(pCtx->iTestRet)

    return 0;
}

/**
 * CallBack helper for Async File Scan to check reported status.
 */
static int ConCbScanCallbackFileAsync(void *pPrivate, int iReason, void *pParam)
{
    ConScanContext *pCtx = (ConScanContext *) pPrivate;
    int iTType = pCtx->iTestType, n = SampleGetCount(iTType);

    switch (iReason)
    {
        case TCS_CB_SCANFINISH:
             CONTEST_START
             CONTEST_ASSERT(pParam != NULL);
             TCSScanResult *pTCSScanResult = pParam;
             CONTEST_ASSERT(pTCSScanResult->iNumDetected == n);
             pTCSScanResult->pfFreeResult(pTCSScanResult);
             CONTEST_RELEASE(pCtx->pConCtx);
             break;

        default:
            break;
    }

    return 0;
}

/**
 * CallBack helper for Async File Scan to cancel and check the reported status.
 */
static int ConCbScanCallbackCancelFileAsync(void *pPrivate, int iReason, void *pParam)
{
    ConScanContext *pCtx = (ConScanContext *) pPrivate;

    switch (iReason)
    {
        case TCS_CB_SCANFINISH:
        {
                CONTEST_START
                int iErr;
                iErr = TCSGetLastError(pCtx->pConCtx->pTestCtx->hLib);
                CONTEST_ASSERT(pParam == NULL);
                CONTEST_RELEASE(pCtx->pConCtx);
        }
                break;

        default:
            //Cancel scan
            return -1;
            break;
    }

    return 0;
}

/**
 * CallBack helper for Sync File Scan cancel.
 */
int CbAbortScanCallbackFileAsync(void *pPrivate, int iReason, void *pParam)
{
	//Cancel scan
    return -1;
}


/**
 * Callback helper for Async data scan cancel and check reported status.
 */
static int ConCbScanCallbackCancelAsync(void *pPrivate, int nReason, void *pParam)
{
    ConScanContext *pCtx = (ConScanContext *) pPrivate;

    switch (nReason)
    {
    	case TCS_CB_SCANFINISH:
    	{
    	        CONTEST_START
    	        int iErr;
    	        iErr = TCSGetLastError(pCtx->pConCtx->pTestCtx->hLib);
	         	CONTEST_ASSERT(pParam == NULL);
	         	CONTEST_RELEASE(pCtx->pConCtx);
    	}
			 	break;
		default:
			//Cancel scan
			return -1;
			break; 
    }

    free(pCtx->pData);
    free(pCtx);
    pCtx = NULL;

    return 0;
}

/**
 * Callback helper for Async data scan, please reference API
 * specification for more information about data scan.
 */
static int ConCbScanCallbackAsync(void *pPrivate, int nReason, void *pParam)
{
    ConScanContext *pCtx = (ConScanContext *) pPrivate;
    const char *pszMalName = NULL, *pszVarName = NULL;
    int i, iTType = pCtx->iTestType, n = SampleGetCount(iTType);

    CONTEST_START

    if (nReason == TCS_CB_SCANFINISH)
    {
        CONTEST_ASSERT(pParam != NULL);
        TCSScanResult *pTCSScanResult = pParam;
        CONTEST_ASSERT(pTCSScanResult->iNumDetected == n)
        pTCSScanResult->pfFreeResult(pTCSScanResult);
        CONTESTREPLY_RELEASE(pCtx->pConCtx);
    }

    if (nReason != TCS_CB_DETECTED)
    {
		//return if not TCS_CB_DETECTED
        return 0;
    }

    CONTEST_ASSERT(nReason == TCS_CB_DETECTED)
    CONTEST_ASSERT(pCtx->iPolarity == INFECTED_DATA)

    pCtx->iInfected++;

    for (i = 0; i < n; i++)
    {
        if (pCtx->pFlags[i])
            continue;
        pszMalName = SampleGetMalName(iTType, i);
        if (pszMalName != NULL)
        {
            CONTEST_ASSERT(pParam != NULL);
            CONTEST_ASSERT(((TCSDetected *) pParam)->pszName != NULL);
            if (((TCSDetected *) pParam)->pszName == NULL ||
                strcmp(((TCSDetected *) pParam)->pszName, pszMalName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        pszVarName = SampleGetVarName(iTType, i);
        if (pszVarName != NULL)
        {
            CONTEST_ASSERT(((TCSDetected *) pParam)->pszVariant != NULL);
            if (((TCSDetected *) pParam)->pszVariant == NULL ||
                strcmp(((TCSDetected *) pParam)->pszVariant, pszVarName) != 0)
                continue;
        }
        else
        {
            continue;
        }
        CONTEST_ASSERT(TCS_ACTION_CLASS(((TCSDetected *) pParam)->uAction) ==
                       SampleGetSeverity(iTType, i));

        CONTEST_ASSERT(TCS_ACTION_BEHAVIOR(((TCSDetected *) pParam)->uAction) ==
                       SampleGetBehavior(iTType, i));

        pCtx->pFlags[i] = 1;
        break;
    }

    CONTEST_ASSERT(i != n);

    CONTEST_RETURN(pCtx->iTestRet);

    return 0;
}

void ConScanData(TestCase *pCtx, int iAction)
{
    int i, iRet = 0;
    ConTestContext ConCtxs[MAX_TEST_THREADS];
    pthread_t Threads[MAX_TEST_THREADS];
    TestCase TestCtx = *pCtx;
    /* Prepare for concurrency tests. */
    ConCreateSampleDirs();
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseCtor(&ConCtxs[i], i + 1, pCtx);

    /* Concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
    {
        if (iAction == TCS_SA_SCANONLY)
            pthread_create(&Threads[i], NULL, ConScanDataSCProc, &ConCtxs[i]);
        else
            pthread_create(&Threads[i], NULL, ConScanDataSRProc, &ConCtxs[i]);
    }

    /* Wait for all tests completed. */
    iRet = ConWaitOnTestCond(&ConCtxs[0]);
    if (iRet == ETIMEDOUT)
    {
        usleep(SLEEP_INTERVAL);
        /* Cancel them all, if timeout. */
        for (i = 0; i < MAX_TEST_THREADS; i++)
        {
            pthread_cancel(Threads[i]);
            /* Wait for cancelling. */
            usleep(SLEEP_INTERVAL);
        }
    }

    for (i = 0; i < MAX_TEST_THREADS; i++)
        pthread_join(Threads[i], NULL);

    /* Check test result. */
    TEST_ASSERT(ConTestSuccess(&ConCtxs[0]) == 1);

    /* Release concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseDtor(&ConCtxs[i]);
    ConDestorySampleDirs();
}

static void *ConScanDataSCProcAsync(void *pConCtxParam)
{
    ConTestContext *pConCtx = (ConTestContext *) pConCtxParam;

    int iAction = TCS_SA_SCANONLY;

    TCSScanParam SP;

    ConScanContext *ScanCtx = (ConScanContext *) malloc(sizeof(ConScanContext));
    char *pszSamplePath = NULL, *pData;
    int i, j, iDataLen = 0, iCid= pConCtx->iCid, iOldType, iExpected;
    int *pFlags;

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    i = MALWARE_TTYPE_BUFFER;

    if ((iExpected = SampleGetCount(i)) > 0)
    {
        pFlags = (int *) malloc(sizeof(int) * iExpected);
    }
    else
        pFlags = NULL;

    j = INFECTED_DATA;

    TestCase TestCtx = *(pConCtx->pTestCtx);
    TEST_ASSERT (pConCtx->pTestCtx->hLib != INVALID_TCSLIB_HANDLE)

    /* Test infected data. */
    pszSamplePath = ConGetSamplePath(i, j, iCid);
    pData = LoadFile(pszSamplePath, &iDataLen);

    ConPutSamplePath(pszSamplePath);
    ScanCtx->pData = pData;
    ScanCtx->uSize = (unsigned int) iDataLen;
    ScanCtx->iTestType = i;
    ScanCtx->iInfected = 0;
    ScanCtx->iPolarity = j;
    ScanCtx->iTestRet = 1;
    ScanCtx->pFlags = pFlags;
    ScanCtx->pConCtx = pConCtx;

    SP.iAction = iAction;
    SP.iDataType = GetSampleDataType(i);
    SP.pPrivate = ScanCtx;
    SP.iCompressFlag = 1;
    SP.pfGetSize = ConCbScanGetSize;
    SP.pfSetSize = ConCbScanSetSize;
    SP.pfRead = ConCbScanRead;
    SP.pfWrite = ConCbScanWrite;
    SP.pfCallBack = pConCtx->pTestCtx->pfCallback;

    if (pFlags)
        memset(pFlags, 0, sizeof(int) * iExpected);

    TEST_ASSERT (TCSScanDataAsync(pConCtx->pTestCtx->hLib, &SP) == 0)

    return NULL;
}

void ConScanDataAsync(TestCase *pCtx, int iAction)
{
    int i, iRet = 0;
    ConTestContext ConCtxs[MAX_TEST_THREADS];
    pthread_t Threads[MAX_TEST_THREADS];

    TCSLIB_HANDLE hLib;
    hLib = TCSLibraryOpen();

    if (iAction == ACTION_SCAN_CANCEL)
        pCtx->pfCallback = ConCbScanCallbackCancelAsync;
    else
		pCtx->pfCallback = ConCbScanCallbackAsync;

    /* Prepare for concurrency tests. */
    ConCreateSampleDirs();
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseCtorAsync(&ConCtxs[i], hLib, i + 1, pCtx);

    /* Concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
    {
       if (pthread_create(&Threads[i], NULL, ConScanDataSCProcAsync, &ConCtxs[i]) != 0)
       {
           Threads[i] = 0;
       }
    }

    /* Wait for all tests completed. */
    iRet = ConWaitOnTestCond(&ConCtxs[0]);
    if (iRet == ETIMEDOUT)
    {
        usleep(SLEEP_INTERVAL);
        /* Cancel them all, if timeout. */
        for (i = 0; i < MAX_TEST_THREADS; i++)
        {
            pthread_cancel(Threads[i]);
            /* Wait for cancelling. */
            usleep(SLEEP_INTERVAL);
        }
    }
    for (i = 0; i < MAX_TEST_THREADS; i++)
    {
        pthread_join(Threads[i], NULL);
    }

    TestCase TestCtx = *pCtx;
    /* Check test result. */
    TEST_ASSERT(ConTestSuccess(&ConCtxs[0]) == 1);

    /* Release concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseDtor(&ConCtxs[i]);
    ConDestorySampleDirs();
    TCSLibraryClose(hLib);
}

static void *ConScanFileSCProc(void *pConCtxParam)
{
    ConTestContext *pConCtx = (ConTestContext *) pConCtxParam;

    ConScanFileProc(pConCtx, TCS_SA_SCANONLY);

    return NULL;
}

static void *ConScanFileSCProcAsync(void *pConCtxParam)
{
    ConTestContext *pConCtx = (ConTestContext *) pConCtxParam;

    int iAction = TCS_SA_SCANONLY;

    char *pszSamplePath = NULL, *pData;
    ConScanContext *ScanCtx = (ConScanContext *) malloc(sizeof(ConScanContext));

    int i, j, iOldType, iCid = pConCtx->iCid, iExpected, iDataLen;
    int *pFlags;

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    i = MALWARE_TTYPE_BUFFER;

    if ((iExpected = SampleGetCount(i)) > 0)
    {
        pFlags = (int *) malloc(sizeof(int) * iExpected);
    }
    else
        pFlags = NULL;

    j = INFECTED_DATA;

    TestCase TestCtx = *(pConCtx->pTestCtx);
    TEST_ASSERT (pConCtx->pTestCtx->hLib != INVALID_TCSLIB_HANDLE)
    /* Test infected data. */
    pszSamplePath = ConGetSamplePath(i, j, iCid);
    pData = LoadFile(pszSamplePath, &iDataLen);
    ConPutSamplePath(pszSamplePath);

    /* Test benign data. */
    pszSamplePath = ConGetSamplePath(i, j, iCid);
    ScanCtx->pData = pData;
    ScanCtx->uSize = (unsigned int) iDataLen;
    ScanCtx->iTestType = i;
    ScanCtx->iInfected = 0;
    ScanCtx->iPolarity = j;
    ScanCtx->iTestRet = 1;
    ScanCtx->pFlags = pFlags;
    ScanCtx->pConCtx = pConCtx;
 
    TEST_ASSERT (TCSScanFileAsync(pConCtx->pTestCtx->hLib, pszSamplePath, GetSampleDataType(i),
                                  iAction, pConCtx->pTestCtx->iCompressFlag, ScanCtx, pConCtx->pTestCtx->pfCallback) == 0);

    return NULL;
}

void ConScanFileAsync(TestCase *pCtx, int iAction)
{
    int i, iRet = 0;
    ConTestContext ConCtxs[MAX_TEST_THREADS];
    pthread_t Threads[MAX_TEST_THREADS];

    TCSLIB_HANDLE hLib;
    hLib = TCSLibraryOpen();

    if (iAction == ACTION_SCAN_CANCEL)
        pCtx->pfCallback = ConCbScanCallbackCancelFileAsync;
    else
       pCtx->pfCallback = ConCbScanCallbackFileAsync;

    /* Prepare for concurrency tests. */
    ConCreateSampleDirs();
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseCtorAsync(&ConCtxs[i], hLib, i + 1, pCtx);

    /* Concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
    {
        pthread_create(&Threads[i], NULL, ConScanFileSCProcAsync, &ConCtxs[i]);
    }

    /* Wait for all tests completed. */
    iRet = ConWaitOnTestCond(&ConCtxs[0]);
    if (iRet == ETIMEDOUT)
    {
        usleep(SLEEP_INTERVAL);
        /* Cancel them all, if timeout. */
        for (i = 0; i < MAX_TEST_THREADS; i++)
        {
            pthread_cancel(Threads[i]);
            /* Wait for cancelling. */
            usleep(SLEEP_INTERVAL);
        }
    }
    for (i = 0; i < MAX_TEST_THREADS; i++)
        pthread_join(Threads[i], NULL);

    TestCase TestCtx = *pCtx;
    /* Check test result. */
    TEST_ASSERT(ConTestSuccess(ConCtxs) == 1);

    /* Release concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseDtor(&ConCtxs[i]);
    ConDestorySampleDirs();
    TCSLibraryClose(hLib);
}

static void *ConScanFileSRProc(void *pConCtxParam)
{
    ConTestContext *pConCtx = (ConTestContext *) pConCtxParam;

    ConScanFileProc(pConCtx, TCS_SA_SCANREPAIR);

    return NULL;
}


static void ConScanFileProc(ConTestContext *pConCtx, int iAction)
{
    TCSLIB_HANDLE hLib;
    TCSScanResult SR = {0};
    char *pszSamplePath = NULL;
    int i, j, iOldType, iCid = pConCtx->iCid, iExpected;
    int *pFlags;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    for (i = MALWARE_TTYPE_BUFFER; i < MALWARE_TTYPE_MULTIPLE; i++)
    {
        if ((iExpected = SampleGetCount(i)) > 0)
        {
            pFlags = (int *) malloc(sizeof(int) * iExpected);
            if (pFlags == NULL)
                CONTEST_ERROR
        }
        else
            pFlags = NULL;

        for (j = BENIGN_DATA; j <= INFECTED_DATA; j++)
        {
            hLib = TCSLibraryOpen();
            if (hLib != INVALID_TCSLIB_HANDLE)
            {
                /* Test benign data. */
                pszSamplePath = ConGetSamplePath(i, j, iCid);

                if (TCSScanFile(hLib, pszSamplePath, GetSampleDataType(i),
                                iAction, pConCtx->pTestCtx->iCompressFlag, &SR) == 0)
                {
                    if (pFlags)
                        memset(pFlags, 0, sizeof(int) * iExpected);

                    if (j == BENIGN_DATA)
                    {
                        CONTEST_ASSERT(SR.iNumDetected == 0)
                    }
                    else
                    {
                        if (iAction == TCS_SA_SCANONLY)
                        {
                            CONTEST_ASSERT(SR.iNumDetected == iExpected &&
                                           ConCheckDetectedList(i, &SR, pFlags) == 0)
                        }
                        else /* Repair */
                        {
                            CONTEST_ASSERT(ConVerifyRepairFile(pszSamplePath, i,
                                           pConCtx->pTestCtx->iCompressFlag) == 0)
                        }
                    }

                    (*SR.pfFreeResult)(&SR);
                }
                else
                {
                    CONTEST_ERROR
                }
                ConPutSamplePath(pszSamplePath);
                TCSLibraryClose(hLib);
            }
            else
            {
                CONTEST_ERROR
            }

            pthread_testcancel();
            usleep(SLEEP_INTERVAL);
            pthread_testcancel();
        }

        if (pFlags)
            free(pFlags);
    }

    CONTEST_RELEASE(pConCtx)
}


static int ConWaitOnTestCond(ConTestContext *pConCtxAry)
{
    int iRet;
    struct timeval Now;
    struct timespec Timeout;

    gettimeofday(&Now, NULL);
    Timeout.tv_sec = Now.tv_sec + DEFAULT_CONCURRENCY_TEST_TIMEOUT;
    Timeout.tv_nsec = Now.tv_usec * 1000;
    iRet = 0;

    pthread_mutex_lock(&g_Mutex);
    while (ConTestComplete(pConCtxAry) != 1 && iRet != ETIMEDOUT)
        iRet = pthread_cond_timedwait(&g_Cond, &g_Mutex, &Timeout);
    pthread_mutex_unlock(&g_Mutex);

    return iRet;
}


void ConScanFile(TestCase *pCtx, int iAction)
{
    int i, iRet = 0;
    ConTestContext ConCtxs[MAX_TEST_THREADS];
    pthread_t Threads[MAX_TEST_THREADS];

    /* Prepare for concurrency tests. */
    ConCreateSampleDirs();
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseCtor(&ConCtxs[i], i + 1, pCtx);

    /* Concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
    {
        if (iAction == TCS_SA_SCANONLY)
            pthread_create(&Threads[i], NULL, ConScanFileSCProc, &ConCtxs[i]);
        else
            pthread_create(&Threads[i], NULL, ConScanFileSRProc, &ConCtxs[i]);
    }
    /* Wait for all tests completed. */
    iRet = ConWaitOnTestCond(&ConCtxs[0]);
    if (iRet == ETIMEDOUT)
    {
        usleep(SLEEP_INTERVAL);
        /* Cancel them all, if timeout. */
        for (i = 0; i < MAX_TEST_THREADS; i++)
        {
            pthread_cancel(Threads[i]);
            /* Wait for cancelling. */
            usleep(SLEEP_INTERVAL);
        }
    }
    for (i = 0; i < MAX_TEST_THREADS; i++)
        pthread_join(Threads[i], NULL);

    TestCase TestCtx = *pCtx;
    /* Check test result. */
    TEST_ASSERT(ConTestSuccess(ConCtxs) == 1);

    /* Release concurrency tests. */
    for (i = 0; i < MAX_TEST_THREADS; i++)
        ConTestCaseDtor(&ConCtxs[i]);
    ConDestorySampleDirs();
}


static char *ConGetSampleDir(int iCid)
{
    int iLen;
    char *pszDir = NULL, *pszRoot = GetTestRoot();

    if (pszRoot != NULL)
    {
        iLen = strlen(pszRoot);
        iLen++; /* Reserved for slash char. */
        iLen += 64; /* Reserved for thread id. */

        pszDir = (char *) calloc(iLen + 1, sizeof(char));
        if (pszDir != NULL)
        {
            if (pszRoot[strlen(pszRoot) - 1] == '/')
                snprintf(pszDir, iLen, "%s%s/t-%d", pszRoot, CONTENTS_ROOT, iCid);
            else
                snprintf(pszDir, iLen, "%s/%s/t-%d", pszRoot, CONTENTS_ROOT, iCid);
        }
        PutTestRoot(pszRoot);
    }

    return pszDir;
}


static void ConPutSampleDir(char *pszDir)
{
    if (pszDir != NULL)
        free(pszDir);
}


static void ConCreateSampleDirs(void)
{
    int i, iLen, iRootLen;
    char *pszRoot = GetTestRoot(), *pszCommand;

    if (pszRoot != NULL)
    {
        for (i = 0; i < MAX_TEST_THREADS; i++)
        {
            iRootLen = strlen(pszRoot);
            iLen = iRootLen * 2;
            iLen += 72; /* Reserved for "mkdir t-" , "cp -f "*/
            pszCommand = (char *) calloc(iLen + 1, sizeof(char));
            if (pszCommand)
            {
                if (pszRoot[iRootLen - 1] == '/')
                    snprintf(pszCommand, iLen, "mkdir -p %s%st-%d", pszRoot, CONTENTS_ROOT, i + 1);
                else
                    snprintf(pszCommand, iLen, "mkdir -p %s/%s/t-%d", pszRoot, CONTENTS_ROOT, i + 1);
                CallSys(pszCommand);

                pszCommand[0] = 0;
                if (pszRoot[iRootLen - 1] == '/')
                    snprintf(pszCommand, iLen, "cp -f %s*.* %s%s/t-%d/ 2>/dev/null",
                             pszRoot, pszRoot, CONTENTS_ROOT, i + 1);
                else
                    snprintf(pszCommand, iLen, "cp -f %s/*.* %s/%s/t-%d/ 2>/dev/null",
                             pszRoot, pszRoot, CONTENTS_ROOT, i + 1);
                CallSys(pszCommand);

                free(pszCommand);
                pszCommand = NULL;
            }
        }
        PutTestRoot(pszRoot);
    }
}


static void ConDestorySampleDirs(void)
{
    int iLen, iRootLen;
    char *pszRoot = GetTestRoot(), *pszCommand;

    if (pszRoot != NULL)
    {
        iRootLen = strlen(pszRoot);
        iLen = iRootLen;
        iLen += 72; /* "rm -rf "*/
        pszCommand = (char *) calloc(iLen + 1, sizeof(char));
        if (pszCommand)
        {
            if (pszRoot[iRootLen - 1] == '/')
                snprintf(pszCommand, iLen, "rm -rf %s%s/t-*", pszRoot, CONTENTS_ROOT);
            else
                snprintf(pszCommand, iLen, "rm -rf %s/%s/t-*", pszRoot, CONTENTS_ROOT);
            CallSys(pszCommand);

            free(pszCommand);
            pszCommand = NULL;
        }
        PutTestRoot(pszRoot);
    }
}


static char *ConGetSamplePath(int iTType, int iPolarity, int iCid)
{
    int iLen, iDirLen;
    const char *pszSampleFileName;
    char *pszSamplePath = NULL, *pszDir = ConGetSampleDir(iCid);
    char *pwd = getenv("PWD");

    if (pszDir != NULL)
    {
        pszSampleFileName = (iPolarity == INFECTED_DATA ?
                             SampleGetInfectedFileName(iTType) :
                             SampleGetBenignFileName(iTType));

        iDirLen = strlen(pszDir);
        iLen = iDirLen;
        iLen += strlen(pszSampleFileName);
        iLen++; /* Reserved for slash char. */
        iLen++; /* Reserved for \0. */
        iLen += strlen(pwd) + 1;

        pszSamplePath = (char *) calloc(iLen + 1, sizeof(char));
        if (pszSamplePath != NULL)
        {
            if (pszDir[iDirLen - 1] == '/')
                snprintf(pszSamplePath, iLen, "%s/%s%s", pwd, pszDir, pszSampleFileName);
            else
                snprintf(pszSamplePath, iLen, "%s/%s/%s", pwd, pszDir, pszSampleFileName);
        }
        ConPutSampleDir(pszDir);
    }

    return pszSamplePath;
}


static void ConPutSamplePath(char *pszSamplePath)
{

    if (pszSamplePath != NULL)
        free(pszSamplePath);
}


int DetectRepairFunc(void)
{
    int iRet = 0; /* Not support by default. */
    TCSScanResult SR = {0};
    char *pszTestSamplePath;
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;

    hLib = TCSLibraryOpen();
    if (hLib != INVALID_TCSLIB_HANDLE)
    {
        pszTestSamplePath = GetBenignSamplePath(MALWARE_TTYPE_BUFFER);
        if (pszTestSamplePath != NULL)
        {
            if (TCSScanFile(hLib, pszTestSamplePath,
                            GetSampleDataType(MALWARE_TTYPE_BUFFER),
                            TCS_SA_SCANREPAIR, 1, &SR) == 0)
            {
                (*SR.pfFreeResult)(&SR);
                iRet = 1; /* Supported. */
            }

            PutBenignSamplePath(pszTestSamplePath);
        }
        TCSLibraryClose(hLib);
    }

    return iRet;
}


int IsTestRepair()
{
    const char *pszType = getenv("TCS_SCAN_TYPE");
    if (pszType != NULL)
        return atoi(pszType);

    return 0;
}


void BackupEngine()
{
    char *pszRoot = GetTestRoot();

    if (pszRoot != NULL)
    {
        char szCommand[1024];

        sprintf(szCommand, "mkdir -p %s/backup", pszRoot);
        CallSys(szCommand);

        sprintf(szCommand, "cp -f /opt/usr/share/sec_plugin/libengine.so %s/backup", pszRoot);
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
        sprintf(szCommand, "cp -f %s/backup/libengine.so /opt/usr/share/sec_plugin/", pszRoot);
        CallSys(szCommand);

        PutTestRoot(pszRoot);
    }
}


static void CallSys(const char *pszCmd)
{
    int iRet = system(pszCmd);
    if (iRet != 0)
    {
        // LOG_OUT("system returns %d for command %s\n", iRet, pszCmd);
    }
}

