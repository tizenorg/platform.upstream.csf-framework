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

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TPCSSerDaemonTest.h"
#include "TPCSSerDaemonTestUtils.h"
#include "TPCSSerDaemon.h"

#ifndef TPCS_DAEMON
#define TPCS_DAEMON "TPCSSerDaemon"
#endif

/* Concurrency test macros. */
/* Asynchronous calls */
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
    ReleaseTestObject(con_test_ctx, iTestRet, 0); \
}

/* Asynchronous reply */
#define CONREPLYTEST_START \
{\
    int iTestReply = 1;

#define CONREPLYTEST_ERROR \
    CONREPLYTEST_ASSERT(0)

#define CONREPLYTEST_ASSERT(condition) \
if (!(condition)) \
{ \
    LOG_OUT("test failed: %s,%d\n", __FILE__, __LINE__); \
    iTestReply = -1; \
}

#define CONREPLYTEST_RETURN(ret) \
    ret = iTestReply; \
}

#define CONREPLYTEST_RELEASE(con_test_ctx) \
    ReleaseTestObject(con_test_ctx, iTestReply, 1); \
}

static void ReportTestCase(TestCase *pCtx);
static void CallSys(const char *pszCmd);
static pid_t StartProcess(const char *szPath, char *const argv[]);
static pid_t StartProcess(const char *szPath, char *const argv[]);
// For test cases needing concurrency.
static void ConTestCaseCtor(ConTestContext *pConCtx, TSC_IPC_HANDLE hIpc, int iCid,
                            TestCase *pCtx, const char *szMethod, int timeout, 
                            TSCCallback pfnCallback);
static void ConTestCaseDtor(ConTestContext *pConCtx);
static int ConWaitOnTestCond(ConTestContext conCtxAry[], int len);
static int ConTestSuccess(ConTestContext conCtxAry[], int len);
static int ConTestComplete(ConTestContext conCtxAry[], int len);
static void ReleaseTestObject(ConTestContext conCtx[], int iResult, int isReply);
static void *ConGetInfoProc(void *pData);
static void *ConGetInfoProcAsync(void *pData);
static void *ConInstallProc(void *pData);
static void *ConInstallProcAsync(void *pData);
static void *ConUninstallProc(void *pData);
static void *ConUninstallProcAsync(void *pData);
static void *ConSetActiveProc(void *pData);
static void *ConSetActiveProcAsync(void *pData);
/*static void *ConSendMessageProc(void *pData);*/
static void *ConSendMessageProcAsync(void *pData);
static void *ConSendMessageProcCancelAsync(void *pData);
static void Callback_GetInfo(void *pPrivate, int argc, const char **argv);
static void Callback_Install(void *pPrivate, int argc, const char **argv);
static void Callback_Uninstall(void *pPrivate, int argc, const char **argv);
static void Callback_SetActive(void *pPrivate, int argc, const char **argv);

pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_Cond = PTHREAD_COND_INITIALIZER;

int TestCasesCount = 0;
int Success = 0;
int Failures = 0;
jmp_buf SCJmpBuf;

/**
 * Output for test case result.
 */
static void ReportTestCase(TestCase *pCtx)
{
    LOG_OUT("Result: Success\n");
}


/**
 * Test case constructor.
 */
void TestCaseCtor(TestCase *pCtx, const char *pszAPI)
{
    strncpy(pCtx->szAPIName, pszAPI, sizeof(pCtx->szAPIName) - 1);

    char *pszTmp;

    LOG_OUT("@@@@@@@@@@@@@@@@@@@@@@@@\n");

    LOG_OUT("@ID: TC_SEC_SC_%s\n", pCtx->szAPIName);
    pszTmp = strchr(pCtx->szAPIName, '_');
    *pszTmp = 0;
    LOG_OUT("@API Name: %s\n", pCtx->szAPIName);
    *pszTmp = '_';

    // Kill any previous instance of Channel-server-stub.
    TerminateAllProcess(TPCS_DAEMON);
}


/**
 * Test case destructor.
 */
void TestCaseDtor(TestCase *pCtx)
{
    ReportTestCase(pCtx);
    Success++;
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


static int ConTestComplete(ConTestContext *pConCtxAry, int len)
{
    int i;
    int count = 0;

    for (i = 0; i < len; i++)
    {
        if (pConCtxAry[i].iConTestRet == 0 || pConCtxAry[i].iConTestReplyRet == 0)
        {
            count++;
            return 0; /* not complete */
        }

    }

    return 1; /* Complete */
}


static int ConTestSuccess(ConTestContext *pConCtxAry, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        DDBG("\nSuccess [%d] iConTestRet=%d, iConTestReplyRet=%d\n", i,
                pConCtxAry[i].iConTestRet, pConCtxAry[i].iConTestReplyRet);

        if (pConCtxAry[i].iConTestRet != 1 || pConCtxAry[i].iConTestReplyRet != 1)
            return 0; /* failure */
    }

    return 1; /* success */
}


static void ConTestCaseCtor(ConTestContext *pConCtx, TSC_IPC_HANDLE hIpc, int iCid, TestCase *pCtx,
                            const char *szMethod, int timeout, TSCCallback pfnCallback)
{
    pConCtx->pTestCtx = pCtx;
    pConCtx->szMethod = szMethod;
    pConCtx->timeout = timeout;
    pConCtx->hIpc = hIpc;
    pConCtx->iCid = iCid;
    pConCtx->cbReply = pfnCallback;
    pConCtx->iConTestRet = 0; /* running. */
    pConCtx->iConTestReplyRet = 0; /* running. */
}


static void ConTestCaseDtor(ConTestContext *pConCtx)
{
}


static void ReleaseTestObject(ConTestContext *pConCtx, int iResult, int isReply)
{
    pthread_mutex_lock(&g_Mutex);
    if (isReply)
        pConCtx->iConTestReplyRet = iResult;
    else
        pConCtx->iConTestRet = iResult;
    pthread_cond_broadcast(&g_Cond);
    pthread_mutex_unlock(&g_Mutex);
}


static int ConWaitOnTestCond(ConTestContext conCtxAry[], int len)
{
    int iRet;
    struct timeval Now;
    struct timespec Timeout;
    int count = 0;

    gettimeofday(&Now, NULL);
    Timeout.tv_sec = Now.tv_sec + DEFAULT_CONCURRENCY_TEST_TIMEOUT;
    Timeout.tv_nsec = Now.tv_usec * 1000;
    iRet = 0;

    pthread_mutex_lock(&g_Mutex);

    while (ConTestComplete(conCtxAry, len) != 1 && iRet != ETIMEDOUT)
    {
        iRet = pthread_cond_timedwait(&g_Cond, &g_Mutex, &Timeout);
        count++;
    }

    pthread_mutex_unlock(&g_Mutex);

    return iRet;
}

/**
 *
 */
void Callback_In1_Out1(void *pPrivate, int argc, char **argv)
{
    CONREPLYTEST_START

    ConTestContext *pConCtx = pPrivate;
    CONREPLYTEST_ASSERT(argc == 1 && (strcmp(argv[0], "1") == 0));

    // Cleanup - On both success and failure.
    CleanupReply(&argv, &argc);
    DDBG("%s\n", "In1_Out1 Callback done");

    CONREPLYTEST_RELEASE(pConCtx)
}


/**
 *
 */
void Callback_In1_Out2(void *pPrivate, int argc, char **argv)
{
    CONREPLYTEST_START

    ConTestContext *pConCtx = pPrivate;
    CONREPLYTEST_ASSERT(argc == 2 && (strcmp(argv[0], "1") == 0));
    CONREPLYTEST_ASSERT(argc == 2 && (strcmp(argv[1], "2") == 0));

    // Cleanup - On both success and failure.
    CleanupReply(&argv, &argc);
    DDBG("%s\n", "In1_Out2 Callback done");

    CONREPLYTEST_RELEASE(pConCtx)
}


static void *ConGetInfoProcAsync(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    TSC_CALL_HANDLE handle = NULL;
    iResult = TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                                  pConCtx->szMethod, req_argc, req_argv,
                                  &handle, Callback_GetInfo, pConCtx, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0);

    CONTEST_RELEASE(pConCtx)

    return NULL;
}

static void *ConGetInfoProc(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    iResult = TSCSendMessageN(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                              pConCtx->szMethod, req_argc, req_argv, &rep_argc,
                              &rep_argv, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0
                   && (rep_argc == 2)
                   && (strcmp(rep_argv[0], RETURN_SUCCESS) == 0));

    CONTEST_RELEASE(pConCtx)

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);

    return NULL;

}


static void *ConInstallProcAsync(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    TSC_CALL_HANDLE handle = NULL;
    iResult = TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                                  pConCtx->szMethod, req_argc, req_argv,
                                  &handle, Callback_Install, pConCtx, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0);

    CONTEST_RELEASE(pConCtx)

    return NULL;
}

static void *ConInstallProc(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_2);
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    iResult = TSCSendMessageN(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                              pConCtx->szMethod, req_argc, req_argv, &rep_argc,
                              &rep_argv, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0
                   && (rep_argc == 2)
                   && (strcmp(rep_argv[0], RETURN_SUCCESS) == 0));

    CONTEST_RELEASE(pConCtx)

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);

    return NULL;

}

static void *ConUninstallProc(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_1);
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    iResult = TSCSendMessageN(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                              pConCtx->szMethod, req_argc, req_argv, &rep_argc,
                              &rep_argv, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0
                   && (rep_argc == 1)
                   && (strcmp(rep_argv[0], RETURN_SUCCESS) == 0));

    CONTEST_RELEASE(pConCtx)

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);

    return NULL;


}
static void *ConUninstallProcAsync(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    TSC_CALL_HANDLE handle = NULL;
    iResult = TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                                  pConCtx->szMethod, req_argc, req_argv,
                                  &handle, Callback_Uninstall, pConCtx, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0);

    CONTEST_RELEASE(pConCtx)

    return NULL;

}


static void *ConSetActiveProc(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_1);
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    iResult = TSCSendMessageN(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                              pConCtx->szMethod, req_argc, req_argv, &rep_argc,
                              &rep_argv, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0
                   && (rep_argc == 1)
                   && (strcmp(rep_argv[0], RETURN_SUCCESS) == 0));

    CONTEST_RELEASE(pConCtx)

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);

    return NULL;


}
static void *ConSetActiveProcAsync(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_1);
    int iResult = -1;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    TSC_CALL_HANDLE handle = NULL;
    iResult = TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                                  pConCtx->szMethod, req_argc, req_argv,
                                  &handle, Callback_SetActive, pConCtx, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0);

    CONTEST_RELEASE(pConCtx)

    return NULL;

}

/*
static void *ConSendMessageProc(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {"1"};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    iResult = TSCSendMessageN(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                              pConCtx->szMethod, req_argc, req_argv, &rep_argc,
                              &rep_argv, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0
                   && (rep_argc == 1)
                   && (strcmp(rep_argv[0], "1") == 0));

    CONTEST_RELEASE(pConCtx)

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);

    return NULL;
}
*/


static void *ConSendMessageProcAsync(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {"1"};
    int iResult = -1;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    TSC_CALL_HANDLE handle = NULL;
    iResult = TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                                  pConCtx->szMethod, req_argc, req_argv,
                                  &handle, pConCtx->cbReply, pConCtx, pConCtx->timeout);

    CONTEST_ASSERT(iResult == 0);

    CONTEST_RELEASE(pConCtx)

    return NULL;
}


static void *ConSendMessageProcCancelAsync(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {"1"};
    int iResult = -1;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    DDBG("%s\n", "Sending message to be CANCELLED");
    TSC_CALL_HANDLE handle = NULL;
    iResult = TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL,
                                  pConCtx->szMethod, req_argc, req_argv,
                                  &handle, pConCtx->cbReply, pConCtx, pConCtx->timeout);

    sleep(1);
    iResult = TSCCancelMessage(pConCtx->hIpc, handle);
    DDBG("Result after cancel:%d\n", iResult);

    CONTEST_ASSERT(iResult == 0);

    CONTEST_RELEASE(pConCtx)

    return NULL;
}


/**
 *
 */
void ConSendMessage(TestCase *pCtx, TSC_IPC_HANDLE hIpc, ConTestContext conCtxs[],
                    pthread_t threads[], int lenCtxs, MethodCall methods[], int timeout)
{
    int i, iRet = 0;

    /* Basic validation */
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    /* Prepare for concurrency tests. */
    for (i = 0; i < lenCtxs; i++)
        ConTestCaseCtor(&conCtxs[i], hIpc, i + 1, pCtx, methods[i].szMethod, timeout,
                        methods[i].pfnCallback);

    /* Concurrency tests. */
    for (i = 0; i < lenCtxs; i++)
    {
        if (methods[i].isAsync == 2)
        {
            /* Async method call then cancel the method */
            pthread_create(&threads[i], NULL, ConSendMessageProcCancelAsync, &conCtxs[i]);
        }
        else if (strcmp(methods[i].szMethod, TPCS_METHOD_INSTALL_PLUGIN) == 0 && methods[i].isAsync== 0)
        {
            conCtxs[i].iConTestReplyRet = 1;
            pthread_create(&threads[i], NULL, ConInstallProc, &conCtxs[i]);
        }
        else if (strcmp(methods[i].szMethod, TPCS_METHOD_INSTALL_PLUGIN) == 0 && methods[i].isAsync== 1)
        {
            conCtxs[i].iConTestReplyRet = 1;
            pthread_create(&threads[i], NULL, ConInstallProcAsync, &conCtxs[i]);
        }
        else if (strcmp(methods[i].szMethod, TPCS_METHOD_UNINSTALL_PLUGIN) == 0 && methods[i].isAsync== 0)
        {
            conCtxs[i].iConTestReplyRet = 1;
            pthread_create(&threads[i], NULL, ConUninstallProc, &conCtxs[i]);
        }
        else if (strcmp(methods[i].szMethod, TPCS_METHOD_UNINSTALL_PLUGIN) == 0 && methods[i].isAsync== 1)
        {
            conCtxs[i].iConTestReplyRet = 1;
            pthread_create(&threads[i], NULL, ConUninstallProcAsync, &conCtxs[i]);
        }
        else if (strcmp(methods[i].szMethod, TPCS_METHOD_SETACTIVE_PLUGIN) == 0 && methods[i].isAsync== 0)
        {
            conCtxs[i].iConTestReplyRet = 1;
            pthread_create(&threads[i], NULL, ConSetActiveProc, &conCtxs[i]);
        }
        else if (strcmp(methods[i].szMethod, TPCS_METHOD_SETACTIVE_PLUGIN) == 0 && methods[i].isAsync== 1)
        {
            conCtxs[i].iConTestReplyRet = 1;
            pthread_create(&threads[i], NULL, ConSetActiveProcAsync, &conCtxs[i]);
        }
        else if (methods[i].isAsync == 1)
        {
            /* Asynchronous method call. */
            pthread_create(&threads[i], NULL, ConGetInfoProcAsync, &conCtxs[i]);
        }
        else if (methods[i].isAsync == 0)
        {
            /* Synchronous method call. */
            conCtxs[i].iConTestReplyRet = 1;
            pthread_create(&threads[i], NULL, ConGetInfoProc, &conCtxs[i]);
        }
    }

    sleep(5);
    /* Wait for all tests completed. */
    iRet = ConWaitOnTestCond(conCtxs, lenCtxs);
    DDBG("All done. iRet = %d\n", iRet);

    if (iRet == ETIMEDOUT)
    {
        DDBG("%s\n", "Timed out.");
        usleep(SLEEP_INTERVAL);
        /* Cancel them all, if timeout. */
        for (i = 0; i < lenCtxs; i++)
        {
            pthread_cancel(threads[i]);
            /* Wait for cancelling. */
            usleep(SLEEP_INTERVAL);
        }
    }
    for (i = 0; i < lenCtxs; i++)
        pthread_join(threads[i], NULL);

    /* Check test result. */
    TEST_ASSERT(ConTestSuccess(conCtxs, lenCtxs) == 1);

    /* Release concurrency tests. */
    for (i = 0; i < lenCtxs; i++)
        ConTestCaseDtor(&conCtxs[i]);
}


/**
 *
 */
void InThreadSendMessageAsync(TestCase *pCtx, TSC_IPC_HANDLE hIpc,
                              ConTestContext conCtxs[], int lenCtxs, 
                              MethodCall methods[], int timeout)
{
    /* Call the async methods. */
    int i = 0;
    for (i = 0; i < lenCtxs; i++)
    {
        ConTestCaseCtor(&conCtxs[i], hIpc, i + 1, pCtx, methods[i].szMethod, timeout,
                        methods[i].pfnCallback);

        // Wraps creating concurrency & sending of message in current thread.
        ConSendMessageProcAsync(&conCtxs[i]);
    }

    /* Wait for all tests to complete. */
    int iRet = ConWaitOnTestCond(conCtxs, lenCtxs);
    DDBG("All done. iRet = %d\n", iRet);

    if (iRet == ETIMEDOUT)
    {
        usleep(SLEEP_INTERVAL);
        // Consider this as failure if the method call has not completed.
    }

    /* Check test result. */
    TEST_ASSERT(ConTestSuccess(conCtxs, lenCtxs) == 1);

    /* Release concurrency tests. */
    for (i = 0; i < lenCtxs; i++)
        ConTestCaseDtor(&conCtxs[i]);
}


/**
 *
 */
void InThreadSendMessageCancelAsync(TestCase *pCtx, TSC_IPC_HANDLE hIpc,
                                    ConTestContext conCtxs[], int lenCtxs,
                                    MethodCall methods[], int timeout)
{
    /* Call the async methods. */
    int i = 0;
    for (i = 0; i < lenCtxs; i++)
    {
        ConTestCaseCtor(&conCtxs[i], hIpc, i + 1, pCtx, methods[i].szMethod, timeout,
                        methods[i].pfnCallback);

        // Wraps creating concurrency & sending of message in current thread.
        // And then sending the Cancel method for it.
        ConSendMessageProcCancelAsync(&conCtxs[i]);
    }

    /* Wait for all tests to complete. */
    int iRet = ConWaitOnTestCond(conCtxs, lenCtxs);
    DDBG("All done. iRet = %d\n", iRet);

    if (iRet == ETIMEDOUT)
    {
        usleep(SLEEP_INTERVAL);
        // Consider this as failure if the method call has not completed.
    }

    /* Check test result. */
    TEST_ASSERT(ConTestSuccess(conCtxs, lenCtxs) == 1);

    /* Release concurrency tests. */
    for (i = 0; i < lenCtxs; i++)
        ConTestCaseDtor(&conCtxs[i]);
}


/**
 * Start the process, passing it an array of args.
 * Returns the process PID.
 */
static pid_t StartProcess(const char *szPath, char *const argv[])
{
    pid_t child_pid = fork();
    if (child_pid == 0)
    {
        execv(szPath, argv);
    }
    //("@@@@@@@@@@@@@@@2222child pid: %d", child_pid);
    //fprintf(stderr, "@@@@@@@@@@@@@@@2222child pid: %d", child_pid);
    return child_pid;
}


/**
 * Terminate the process gracefully.
 * Returns 0 on success, non-zero on failure.
 */
int TerminateProcess(pid_t pid)
{
    if (pid)
        return kill(pid, SIGTERM);

    return 1;
}


/**
 * Terminate all the the process instances with the given name.
 */
void TerminateAllProcess(const char *szName)
{
    char szKillAll[1040] = {0};
    snprintf(szKillAll, 1039, "killall %s 2>/dev/null", szName);
    return CallSys(szKillAll);
}

pid_t AddPlugApp(void)
{
    //char *args[] = {" -r", "/tmp/tsc_test/q7097a278m", "/opt/usr/apps/", NULL};
    char *args[] = {"cp", "-r", "/tmp/tsc_test/q7097a278m", "/opt/usr/apps/ 2>/dev/null ", NULL};
    pid_t pid = StartProcess("/bin/cp", args);
    sleep(1);
    return pid;
}

pid_t StartTPCSServerStub(void)
{
    char *args[] = {TPCS_DAEMON, NULL};
    pid_t pid = StartProcess("/usr/bin/"TPCS_DAEMON, args);

    // let server start running.
    sleep(1);
    return pid;
}


void CleanupReply(char ***pArr, int *pLen)
{
    if (pArr && *pArr) {
        while (*pLen)
            free ((*pArr)[--(*pLen)]);

        free(*pArr);
        *pArr = NULL;
    }
}

static void GetNSURI(xmlDoc *pXmlDoc, xmlChar **pns, xmlChar **uri)
{
    xmlNode *root_node = xmlDocGetRootElement(pXmlDoc);

    if (root_node)
    {
        xmlNs *pXmlNS = root_node->ns;
        xmlNs *pXmlNSDef = root_node->nsDef;

        if (pXmlNS != NULL)
        {
            if (pXmlNS->prefix != NULL)
            {
                *pns = (xmlChar*) strdup((const char*) pXmlNS->prefix);
            }
            else
            {
                *pns = (xmlChar*) strdup((const char*) root_node->name);
            }
        }

        if (pXmlNSDef != NULL)
        {
            if (pXmlNSDef->href != NULL)
            {
                *uri = (xmlChar*) strdup((const char*) pXmlNSDef->href);
            }
        }
    }
    return;
}

int GetXmlFromMemory(xmlDoc **pXmlDoc, const char *xmlInMemory)
{
    int result = -1;

    *pXmlDoc = xmlReadMemory(xmlInMemory, sizeof(xmlInMemory) + 1,
                             "/usr/bin/tpcs_config.xml", NULL, 0);
    if (*pXmlDoc != NULL)
    {
        result = 0;
    }


    return result;
}

int GetXmlFromFile(const char *pFileName, xmlDoc **pXmlDoc)
{
    int ret = -1;
    xmlParserCtxt *pParserCtxt = xmlNewParserCtxt();
    if (pParserCtxt != NULL)
    {
        // valid this config file
        FILE *fpConfig = fopen(pFileName, "r");
        if (fpConfig)
        {
            fclose(fpConfig); // File exists
            // Parse the file, activating the DTD validation option */
            *pXmlDoc = xmlCtxtReadFile(pParserCtxt, pFileName, NULL, XML_PARSE_DTDVALID);
            if (pXmlDoc != NULL)
            {
                // Check if validation succeeded
                if (pParserCtxt->valid)
                    ret = 0;
            }
        }

        xmlFreeParserCtxt(pParserCtxt);
    }

    return ret;
}

int SearchNodeN(const xmlChar *xpath, xmlDocPtr pXmlDoc, const char* appId)
{

    int result = -1;
    xmlXPathContext *pXPathCtx;
    xmlXPathObject *pXPathObj;

    xmlNodeSet *pNodeSet;
    pXPathCtx = xmlXPathNewContext(pXmlDoc);
    if (pXPathCtx == NULL)
        return result;

    xmlChar *pns = NULL;
    xmlChar *puri = NULL;
    GetNSURI(pXmlDoc, &pns, &puri);

    if (pns != NULL && puri != NULL)
    {
        if (xmlXPathRegisterNs(pXPathCtx, pns, puri) != 0)
            return result;
    }

    //Evaluate xpath expression
    pXPathObj = xmlXPathEvalExpression(xpath, pXPathCtx);

    if (pXPathObj == NULL)
    {

        free(pns);
        free(puri);
        return result;
    }

    pNodeSet = pXPathObj->nodesetval;
    if (pNodeSet)
    {
        int count = pNodeSet->nodeNr;
        int i = 0;
        xmlNode *cur;

        for(; i < count; i++)
        {
        	if (pNodeSet->nodeTab[i])
        	{
                cur = pNodeSet->nodeTab[i]->children;
                if (cur)
                {
                    if (strcmp((const char *)cur->content, appId) == 0)
                    {
                        result = 0;
                        break;
                    }

                }

        	}
        }

    }
    xmlXPathFreeObject(pXPathObj);
    xmlXPathFreeContext(pXPathCtx);
    return result;

}

int HasNode(const char *xpath, const char *appId)
{

    int ret = -1;
    xmlDoc *pDoc = NULL;

    ret = GetXmlFromFile("/usr/bin/tpcs_config.xml", &pDoc);
    if (ret == 0)
    {
        ret = SearchNodeN((const xmlChar *)xpath, pDoc, appId);
        xmlFreeDoc(pDoc);
    }
    return ret;
}

/**
 *
 */
static void Callback_GetInfo(void *pPrivate, int argc, const char **argv)
{
    CONREPLYTEST_START

    ConTestContext *pConCtx = pPrivate;

    if (argc == 2)
    {
        CONREPLYTEST_ASSERT(strncmp(argv[0], RETURN_SUCCESS, 1) == 0);
    }

    DDBG("%s\n", "Callback done");

    CONREPLYTEST_RELEASE(pConCtx)
}

static void Callback_Install(void *pPrivate, int argc, const char **argv)
{
    CONREPLYTEST_START

    ConTestContext *pConCtx = pPrivate;

    if (argc == 2)
    {
        CONREPLYTEST_ASSERT(strncmp(argv[0], RETURN_SUCCESS, 1) == 0);
    }

    DDBG("%s\n", "Callback done");

    CONREPLYTEST_RELEASE(pConCtx)
}

static void Callback_Uninstall(void *pPrivate, int argc, const char **argv)
{
    CONREPLYTEST_START

    ConTestContext *pConCtx = pPrivate;

    if (argc == 2)
    {
        CONREPLYTEST_ASSERT(strncmp(argv[0], RETURN_SUCCESS, 1) == 0);
    }

    DDBG("%s\n", "Callback done");

    CONREPLYTEST_RELEASE(pConCtx)
}

static void Callback_SetActive(void *pPrivate, int argc, const char **argv)
{
    CONREPLYTEST_START

    ConTestContext *pConCtx = pPrivate;

    if (argc == 2)
    {
        CONREPLYTEST_ASSERT(strncmp(argv[0], RETURN_SUCCESS, 1) == 0);
    }

    DDBG("%s\n", "Callback done");

    CONREPLYTEST_RELEASE(pConCtx)
}
