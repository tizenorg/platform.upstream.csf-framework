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

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "TWPImpl.h"
#include "TWPSerDaemon.h"
#include "TWPSerDaemonTest.h"
#include "TWPSerDaemonTestUtils.h"

#ifndef SERVER_STUB
#define SERVER_STUB "TWPSerDaemon"
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
    LOG_OUT("test failed: %s at %s:%d\n", TestCtx.szAPIName, __FILE__, __LINE__); \
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
                            TestCase *pCtx, const char *szMethod, int timeout);
static void ConTestCaseDtor(ConTestContext *pConCtx);
static int ConWaitOnTestCond(ConTestContext conCtxAry[], int len);
static int ConTestSuccess(ConTestContext conCtxAry[], int len);
static int ConTestComplete(ConTestContext conCtxAry[], int len);
static void ReleaseTestObject(ConTestContext conCtx[], int iResult, int isReply);
static void *ConSendMessageProc(void *pData);
static void *ConSendMessageProcAsync(void *pData);

pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_Cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t g_Mutex_cb = PTHREAD_MUTEX_INITIALIZER;

int TestCasesCount = 0;
int Success = 0;
int Failures = 0;
jmp_buf SCJmpBuf;

/**
 * Output for test case result.
 */
static void ReportTestCase(TestCase *pCtx)
{
    LOG_OUT("@@@@@@@@@@End Test: %s @@@@@@@@@@@@@@\n", pCtx->szAPIName);
    LOG_OUT("Result: Success\n");
}


/**
 * Test case constructor.
 */
void TestCaseCtor(TestCase *pCtx, const char *pszAPI)
{
    strncpy(pCtx->szAPIName, pszAPI, sizeof(pCtx->szAPIName) - 1);

    char *pszTmp;

    LOG_OUT("@@@@@@@@@@Start Test: %s @@@@@@@@@@@@@@\n", pCtx->szAPIName);

    LOG_OUT("@ID: TC_SEC_SC_%s\n", pCtx->szAPIName);
    pszTmp = strchr(pCtx->szAPIName, '_');
    *pszTmp = 0;
    LOG_OUT("@API Name: %s\n", pCtx->szAPIName);
    *pszTmp = '_';

    // Kill any previous instance of Channel-server-stub.
    TerminateAllProcess(SERVER_STUB);
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
    int count;

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

        if (pConCtxAry[i].iConTestRet != 1 || pConCtxAry[i].iConTestReplyRet != 1)
            return 0; /* failure */
    }

    return 1; /* success */
}


static void ConTestCaseCtor(ConTestContext *pConCtx, TSC_IPC_HANDLE hIpc, int iCid, TestCase *pCtx,
                            const char *szMethod, int timeout)
{
    pConCtx->pTestCtx = pCtx;
    pConCtx->szMethod = szMethod;
    pConCtx->timeout = timeout;
    pConCtx->hIpc = hIpc;
    pConCtx->iCid = iCid;
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
    {
        if (iResult == 0)
        {
            LOG_OUT("Test Failed");
        }
        pConCtx->iConTestReplyRet = iResult;
    }
    else
    {
        pConCtx->iConTestRet = iResult;
    }
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
static void Callback_GetVer(void *pPrivate, int argc, const char **argv)
{
    pthread_mutex_lock(&g_Mutex_cb);
    CONREPLYTEST_START
    ConTestContext *pConCtx = pPrivate;

    if (argc == 4)
    {
        CONREPLYTEST_ASSERT(strncmp(argv[0], "0", 1) == 0);
        CONREPLYTEST_ASSERT(strncmp(argv[1], TWP_FRAMEWORK_VERSION, strlen(TWP_FRAMEWORK_VERSION)) == 0);
        CONREPLYTEST_ASSERT(strlen(argv[2]) > 0);
        CONREPLYTEST_ASSERT(strncmp(argv[3], TWP_DAEMON_VERSION, strlen(TWP_DAEMON_VERSION)) == 0);
    }

    if (argc == 1 || argc == 0)
    {
        DDBG("%s\n", "Debug: Error returned from channel client");
    }

    DDBG("%s\n", "Callback done");
    CONREPLYTEST_RELEASE(pConCtx)
    pthread_mutex_unlock(&g_Mutex_cb);
}


/**
 *
 */
static void Callback_GetRep(void *pPrivate, int argc, const char **argv)
{
    pthread_mutex_lock(&g_Mutex_cb);
    CONREPLYTEST_START
    ConTestContext *pConCtx = pPrivate;

    if (argc == 3)
    {
        CONREPLYTEST_ASSERT(strncmp(argv[0], "0", 1) == 0);
        CONREPLYTEST_ASSERT(strncmp(argv[1], Test_TWP_High, 1) == 0);
        CONREPLYTEST_ASSERT(strlen(argv[2]) > 0);
    }

    if (argc == 0 || argc == 1)
    {
        DDBG("%s\n", "Debug: Error returned from channel client");
    }

    DDBG("%s\n", "Callback done");
    sleep(2);
    CONREPLYTEST_RELEASE(pConCtx)
    pthread_mutex_unlock(&g_Mutex_cb);
}

static void *ConSendMessageProc(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;
    TestCase TestCtx = *(pConCtx->pTestCtx);
    // Request args.
    int req_argc = 1;
    char *req_argv[] = {"www.zcrack.com"};

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    CONTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    if (TSCSendMessageN(pConCtx->hIpc, TSC_DBUS_SERVER_WP_CHANNEL, pConCtx->szMethod, req_argc, req_argv, &rep_argc,
                              &rep_argv, pConCtx->timeout) == 0)
    {
        if (strcmp(pConCtx->szMethod, TWPGETVERSIONMETHOD) == 0)
        {
            if (rep_argc == 4)
            {
                CONTEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
                CONTEST_ASSERT(strncmp(rep_argv[1], TWP_FRAMEWORK_VERSION, strlen(TWP_FRAMEWORK_VERSION)) == 0);
                CONTEST_ASSERT(strlen(rep_argv[2]) > 0);
                CONTEST_ASSERT(strncmp(rep_argv[3], TWP_DAEMON_VERSION, strlen(TWP_DAEMON_VERSION)) == 0);
            }
            else
            {
                DDBG("Debug: call did not succeed with result code %s", rep_argv[0]);
            }
        }

        if (strcmp(pConCtx->szMethod, TWPGETURLREPUTATIONMETHOD) == 0)
        {
            if (rep_argc == 3)
            {
                CONTEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
                CONTEST_ASSERT(strncmp(rep_argv[1], Test_TWP_High, 1) == 0);
                CONTEST_ASSERT(strlen(rep_argv[2]) > 0);
            }
            else
            {
                DDBG("Debug: call did not succeed with result code %s", rep_argv[0]);
            }
            sleep(3);
        }
    }
    else
    {
        DDBG("Error while sending message");
    }

    CONTEST_RELEASE(pConCtx)

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);

    return NULL;
}


static void *ConSendMessageProcAsync(void *pData)
{
    int iOldType;
    ConTestContext *pConCtx = (ConTestContext *) pData;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {"www.zcrack.com"};

    CONREPLYTEST_START

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &iOldType);

    TSC_CALL_HANDLE handle = NULL;

    if (strcmp(pConCtx->szMethod, TWPGETVERSIONMETHOD) == 0)
    {
        if (TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_WP_CHANNEL, pConCtx->szMethod, req_argc, req_argv,
                                      &handle, Callback_GetVer, pConCtx, pConCtx->timeout) != 0)
            DDBG("Debug: Error while sending message TSCSendMessageAsync\n");
        
        sleep(4);
    }

    if (strcmp(pConCtx->szMethod, TWPGETURLREPUTATIONMETHOD) == 0)
    {
        if (TSCSendMessageAsync(pConCtx->hIpc, TSC_DBUS_SERVER_WP_CHANNEL, pConCtx->szMethod, req_argc, req_argv,
                                      &handle, Callback_GetRep, pConCtx, pConCtx->timeout) != 0)
            DDBG("Debug: Error while sending message TSCSendMessageAsync\n");

        sleep(3);
    }

    CONREPLYTEST_RELEASE(pConCtx)

    return NULL;
}


/**
 *
 */
void ConSendMessage(TestCase *pCtx, TSC_IPC_HANDLE hIpc, ConTestContext conCtxs[],
                    pthread_t threads[], int lenCtxs, MethodCall methods[], int timeout)
{
    int i, iRet = 0;

    TestCase TestCtx = *pCtx;

    /* Basic validation */
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    /* Prepare for concurrency tests. */
    for (i = 0; i < lenCtxs; i++)
        ConTestCaseCtor(&conCtxs[i], hIpc, i + 1, pCtx, methods[i].szMethod, timeout);

    /* Concurrency tests. */
    for (i = 0; i < lenCtxs; i++)
    {
        if (methods[i].isAsync)
        {
            conCtxs[i].iConTestRet = 1;
            /* Asynchronous method call. */
            if (pthread_create(&threads[i], NULL, ConSendMessageProcAsync, &conCtxs[i]) != 0)
            {
                threads[i] = 0;
            }
        }
        else
        {
            /* Synchronous method call. */
            conCtxs[i].iConTestReplyRet = 1;
            if (pthread_create(&threads[i], NULL, ConSendMessageProc, &conCtxs[i]) != 0)
            {
                threads[i] = 0;
            }
        }
    }

    /* Wait for all tests completed. */
    iRet = ConWaitOnTestCond(conCtxs, lenCtxs);

    if (iRet == ETIMEDOUT)
    {
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


pid_t StartServerStub(void)
{
    char *args[] = {SERVER_STUB, NULL};
    pid_t pid = StartProcess("/usr/bin/"SERVER_STUB, args);

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

void BackupEngine()
{
    char *pszRoot = GetTestRoot();

    if (pszRoot != NULL)
    {
        char szCommand[1024];

        sprintf(szCommand, "mkdir -p %s/backup", pszRoot);
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
        char szCommand[1024] = {0};

        sprintf(szCommand, "cp -f %s/backup/libwpengine.so /opt/usr/share/sec_plugin/", pszRoot);
        CallSys(szCommand);

        PutTestRoot(pszRoot);
    }
}

void RemoveEngine()
{
    LOG_OUT("Remove Engine");
    char *pszRoot = GetTestRoot();

    BackupEngine();
    if (pszRoot != NULL)
    {
        LOG_OUT("Remove Engine 1");
        char szCommand[1024] = "rm -f /opt/usr/share/sec_plugin/libwpengine.so";
        CallSys(szCommand);
        PutTestRoot(pszRoot);
    }
}
