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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "TWPSerDaemon.h"
#include "TWPImpl.h"
#include "TWPSerDaemonTest.h"
#include "TSCTestCommon.h"
#include "TWPSerDaemonTestUtils.h"
#include "UrlInfo.h"

/* Constants */
#define DEF_TIMEOUT -1
#define TIMES 10

/* Test cases. */
static void TWPSerDaemonStartup(void);
static void TWPSerDaemonCleanup(void);
static void TWPSerDaemonGetVerSendMessageN_001(void);
static void TWPSerDaemonGetVerSendMessageN_002(void);
static void TWPSerDaemonGetVerSendMessageN_003(void);
static void TWPSerDaemonGetVerSendMessageN_004(void);
static void TWPSerDaemonGetVerSendMessageSync_001(void);
static void TWPSerDaemonGetVerSendMessageAsync_001(void);
static void TWPSerDaemonGetRepSendMessageN_001(void);
static void TWPSerDaemonGetRepSendMessageN_002(void);
static void TWPSerDaemonGetRepSendMessageN_003(void);
static void TWPSerDaemonGetRepSendMessageN_004(void);
static void TWPSerDaemonGetRepSendMessageN_005(void);
static void TWPSerDaemonGetRepSendMessageN_006(void);
static void TWPSerDaemonGetRepSendMessageN_007(void);
static void TWPSerDaemonGetRepSendMessageAsync_001(void);
static void TWPSerDaemonGetRepSendMessageAsync_002(void);
static void TWPSerDaemonGetRepGetVerSendAsync_001(void);

static void TestCases(void);

/* Unit Test Cases END. */

extern int TestCasesCount;
extern int Success;
extern int Failures;


int main(int argc, char **argv)
{
    TWPSerDaemonStartup();
    TestCases();
    TWPSerDaemonCleanup();

    return 0;
}

static void TestCases(void)
{
    TWPSerDaemonGetVerSendMessageN_001();
    TWPSerDaemonGetVerSendMessageN_002();
    TWPSerDaemonGetVerSendMessageN_003();
    TWPSerDaemonGetVerSendMessageN_004();
    TWPSerDaemonGetVerSendMessageSync_001();
    TWPSerDaemonGetVerSendMessageAsync_001();
    TWPSerDaemonGetRepSendMessageN_001();
    TWPSerDaemonGetRepSendMessageN_002();
    TWPSerDaemonGetRepSendMessageN_003();
    TWPSerDaemonGetRepSendMessageN_004();
    TWPSerDaemonGetRepSendMessageN_005();
    TWPSerDaemonGetRepSendMessageN_006();
    TWPSerDaemonGetRepSendMessageN_007();
    TWPSerDaemonGetRepSendMessageAsync_001();
    TWPSerDaemonGetRepSendMessageAsync_002();
    TWPSerDaemonGetRepGetVerSendAsync_001();
}

/**
 * Test sending message TWPGETVERSIONMETHOD without running server.
 */
static void TWPSerDaemonGetVerSendMessageN_001(void)
{
    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 0;
    char *req_argv[] = {};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();
    TESTCASECTOR(&TestCtx, __FUNCTION__);

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    /* Sending message without server should return failure. */
    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETVERSIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult != 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
}


/**
 * Test sending message TWPGETVERSIONMETHOD with server running.
 */
static void TWPSerDaemonGetVerSendMessageN_002(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 0;
    char *req_argv[] = {};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETVERSIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT((iResult == 0) && (rep_argc == 4));
    TEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], TWP_FRAMEWORK_VERSION, strlen(TWP_FRAMEWORK_VERSION)) == 0);
    TEST_ASSERT(strlen(rep_argv[2]) > 0);
    TEST_ASSERT(strncmp(rep_argv[3], TWP_DAEMON_VERSION, strlen(TWP_DAEMON_VERSION)) == 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}


/**
 * Test sending TWPGETVERSIONMETHOD message more than once, with the TWPSerDaemon server running.
 */
static void TWPSerDaemonGetVerSendMessageN_003(void)
{
    int i = 0;
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 0;
    char *req_argv[] = {};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);
    LOG_OUT("Verbose: Please wait for test case to complete\n");
    for (i = 0; i < TIMES; ++i)
    {
        iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETVERSIONMETHOD,
                                  req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

        TEST_ASSERT((iResult == 0) && (rep_argc == 4));
        TEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
        TEST_ASSERT(strncmp(rep_argv[1], TWP_FRAMEWORK_VERSION, strlen(TWP_FRAMEWORK_VERSION)) == 0);
        TEST_ASSERT(strlen(rep_argv[2]) > 0);
        TEST_ASSERT(strncmp(rep_argv[3], TWP_DAEMON_VERSION, strlen(TWP_DAEMON_VERSION)) == 0);
        CleanupReply(&rep_argv, &rep_argc);
        DDBG("Sending Message to get Version (%d/%d)\n", i + 1, TIMES);
    }

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * return error code from TWPSerDaemon if libwpengine.so is not present.
 */
static void TWPSerDaemonGetVerSendMessageN_004(void)
{

    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 0;
    char *req_argv[] = {};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    BackupEngine();
    system("rm -f /opt/usr/share/sec_plugin/libwpengine.so");
    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETVERSIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);
    RestoreEngine();

    TEST_ASSERT((iResult == 0) && (rep_argc == 1));
    TEST_ASSERT(strncmp(rep_argv[0], "3", 1) == 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * Multi-Thread Test case: [Sync method] Verify if we are able to do sync TWPGETVERSION in
 * the 10 threads.
 */
static void TWPSerDaemonGetVerSendMessageSync_001(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TWPGETVERSIONMETHOD);
        methods[i].isAsync = 0;
    }

    ConSendMessage(&TestCtx, hIpc, ConCtxs, Threads, TIMES, methods, DEF_TIMEOUT);


    for (i = 0; i < TIMES; i++)
    {
        free(methods[i].szMethod);
    }

    TESTCASEDTOR(&TestCtx);

    IpcClientClose(hIpc);

    TerminateProcess(pidStub);
}

/**
 * Multi-Thread Test case: [Async method] Verify if we are able to do async TWPGETVERSION in
 * the 2 threads.
 */
static void TWPSerDaemonGetVerSendMessageAsync_001(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TWPGETVERSIONMETHOD);
        methods[i].isAsync = 1;
    }

    ConSendMessage(&TestCtx, hIpc, ConCtxs, Threads, TIMES, methods, DEF_TIMEOUT);

    for (i = 0; i < TIMES; i++)
    {
        free(methods[i].szMethod);
    }

    TESTCASEDTOR(&TestCtx);

    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}


/**
 * Test sending TWPGETURLREPUTATIONMETHOD message with TWPSerDaemon not running.
 */
static void TWPSerDaemonGetRepSendMessageN_001(void)
{
    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {URL_3_0};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();
    TESTCASECTOR(&TestCtx, __FUNCTION__);

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETURLREPUTATIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT((iResult != 0));

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
}


/**
 * Test sending TWPGETURLREPUTATIONMETHOD message with the TWPSerDaemon server running.
 * Tests URL with TWP_High risk level.
 */
static void TWPSerDaemonGetRepSendMessageN_002(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {URL_3_0};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETURLREPUTATIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT((iResult == 0) && (rep_argc == 3));
    TEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], Test_TWP_High, 1) == 0);
    TEST_ASSERT(strlen(rep_argv[2]) > 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * Test sending TWPGETURLREPUTATIONMETHOD message with the TWPSerDaemon server running.
 * Tests URL with TWP_Medium risk level.*
 */
static void TWPSerDaemonGetRepSendMessageN_003(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {URL_4_0};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETURLREPUTATIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT((iResult == 0) && (rep_argc == 3));
    TEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], Test_TWP_Medium, 1) == 0);
    TEST_ASSERT(strlen(rep_argv[2]) > 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * Test sending TWPGETURLREPUTATIONMETHOD message with the TWPSerDaemon server running.
 * Tests URL with TWP_Unverified risk level.
 */
static void TWPSerDaemonGetRepSendMessageN_004(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {URL_2_0};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETURLREPUTATIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT((iResult == 0) && (rep_argc == 2));
    TEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], Test_TWP_Unverified, 1) == 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * Test sending TWPGETURLREPUTATIONMETHOD message with the TWPSerDaemon server running.
 * Tests URL with TWP_Minimal risk level.
 */
static void TWPSerDaemonGetRepSendMessageN_005(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {URL_1_0};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETURLREPUTATIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT((iResult == 0) && (rep_argc == 2));
    TEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], Test_TWP_Minimal, 1) == 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * Check return code from TWPSerDaemon if libwpengine.so is not present
 */
static void TWPSerDaemonGetRepSendMessageN_006(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {URL_3_0};
    int iResult = -1;

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);
    BackupEngine();
    system("rm -f /opt/usr/share/sec_plugin/libwpengine.so");
    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETURLREPUTATIONMETHOD,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);
    RestoreEngine();
    TEST_ASSERT((iResult == 0) && (rep_argc == 1));
    TEST_ASSERT(strncmp(rep_argv[0], "3", 1) == 0);

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * Test sending TWPGETURLREPUTATIONMETHOD message more than once, with the TWPSerDaemon server running.
 */
static void TWPSerDaemonGetRepSendMessageN_007(void)
{
    int i = 0;
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    // Request args.
    int req_argc = 1;
    char *req_argv[] = {URL_3_0};

    // Response args.
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);
    LOG_OUT("Verbose: Please wait for test case to complete\n");

    for (i = 0; i < TIMES; ++i)
    {
        DDBG("Debug: Sending message to get URL reputation (%d/%d)\n", i + 1, TIMES);
        if (TSCSendMessageN(hIpc, TSC_DBUS_SERVER_WP_CHANNEL, TWPGETURLREPUTATIONMETHOD,
                                  req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT) == 0)
		{
		    if (rep_argc == 3)
		    {
		        TEST_ASSERT(strncmp(rep_argv[0], "0", 1) == 0);
		        TEST_ASSERT(strncmp(rep_argv[1], Test_TWP_High, 1) == 0);
		        TEST_ASSERT(strlen(rep_argv[2]) > 0);
		    }
		    else
		    {
		        DDBG("Debug: Error while receiving the reputation");
		    }
		}
		else
		{
   	       DDBG("Debug: Error while Sending message\n");
		}

        CleanupReply(&rep_argv, &rep_argc);
        sleep(3);
    }

    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}


/**
 * Multi-Thread Test case: [Sync method] Verify if we are able to do sync TWPGETURLREPUTATIONMETHOD in
 * the 2 threads.
 */
static void TWPSerDaemonGetRepSendMessageAsync_001(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TWPGETURLREPUTATIONMETHOD);
        methods[i].isAsync = 0;
    }

    ConSendMessage(&TestCtx, hIpc, ConCtxs, Threads, TIMES, methods, DEF_TIMEOUT);

    for (i = 0; i < TIMES; i++)
    {
        free(methods[i].szMethod);
    }

    TESTCASEDTOR(&TestCtx);

    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * Multi-Thread Test case: [Async method] Verify if we are able to do async TWPGETURLREPUTATIONMETHOD in
 * the 2 threads.
 */
static void TWPSerDaemonGetRepSendMessageAsync_002(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TWPGETURLREPUTATIONMETHOD);
        methods[i].isAsync = 1;
    }

    ConSendMessage(&TestCtx, hIpc, ConCtxs, Threads, TIMES, methods, DEF_TIMEOUT);

    TESTCASEDTOR(&TestCtx);

    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

static void TWPSerDaemonGetRepGetVerSendAsync_001(void)
{
    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartServerStub();
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i = i + 2)
    {
        methods[i].szMethod = strdup(TWPGETURLREPUTATIONMETHOD);
        methods[i].isAsync = 1;
    }

    for (i = 1; i < TIMES; i = i + 2)
    {
        methods[i].szMethod = strdup(TWPGETVERSIONMETHOD);
        methods[i].isAsync = 1;
    }

    ConSendMessage(&TestCtx, hIpc, ConCtxs, Threads, TIMES, methods, DEF_TIMEOUT);

    for (i = 0; i < TIMES; i++)
    {
        free(methods[i].szMethod);
    }

    TESTCASEDTOR(&TestCtx);

    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

static void TWPSerDaemonStartup(void)
{
    extern int TestCasesCount;
    extern int Success;
    extern int Failures;

    TestCasesCount = 0;
    Success = 0;
    Failures = 0;
}


static void TWPSerDaemonCleanup(void)
{
    LOG_OUT("@@@@@@@@@@@@@@@@@@@@@@@@\n");
    LOG_OUT("Test done: %d executed, %d passed, %d failure\n", TestCasesCount, Success, Failures);
}
