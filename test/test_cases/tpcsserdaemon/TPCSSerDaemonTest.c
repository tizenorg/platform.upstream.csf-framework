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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <libxml/parser.h>

#include "TPCSSerDaemonTest.h"
#include "TPCSSerDaemonTestUtils.h"
#include "TSCErrorCodes.h"
#include "TPCSSerDaemon.h"

/**
 * Predefined method
 */

static void WriteToFileFromMemory(const char *data, const char *file_name_w_path);
static void WriteToMemoryFromFile(xmlChar **data, int *size, const char *file_name_w_path);
int
FetchList(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    return 0;
}

int
UpdateList(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    return 0;
}

/* Constants */
#define DEF_TIMEOUT -1


/* Test cases. */
static void TSCStartup(void);
static void TSCCleanup(void);
//static void TestCases(void);

/**
 *  Plugin Control Service Test Cases BEGIN
 */
static void TPCS_GetPluginInfo_001(void);
static void TPCS_GetPluginInfo_002(void);
static void TPCS_GetPluginInfo_003(void);
static void TPCS_GetPluginInfo_004(void);
static void TPCS_GetPluginInfo_005(void);
static void TPCS_GetPluginInfo_006(void);
static void TPCS_GetPluginInfoSync_001();
static void TPCS_GetPluginInfoAsync_001();
static void TPCS_InstallPlugin_001(void);
static void TPCS_InstallPlugin_002(void);
static void TPCS_InstallPlugin_003(void);
static void TPCS_InstallPlugin_004(void);
static void TPCS_InstallPlugin_005(void);
static void TPCS_InstallPlugin_006(void);
static void TPCS_InstallPlugin_007(void);
static void TPCS_InstallPlugin_008(void);
static void TPCS_InstallPlugin_009(void);
static void TPCS_InstallPlugin_010(void);
static void TPCS_InstallPlugin_011(void);
static void TPCS_InstallPlugin_012(void);
static void TPCS_InstallPlugin_013(void);
static void TPCS_InstallPluginSync_001(void);
static void TPCS_InstallPluginAsync_001(void);
static void TPCS_UninstallPlugin_001(void);
static void TPCS_UninstallPlugin_002(void);
static void TPCS_UninstallPlugin_003(void);
static void TPCS_UninstallPlugin_004(void);
static void TPCS_UninstallPluginSync_001(void);
static void TPCS_UninstallPluginAsync_001(void);
static void TPCS_SetActivePlugin_001(void);
static void TPCS_SetActivePlugin_002(void);
static void TPCS_SetActivePlugin_003(void);
static void TPCS_SetActivePluginSync_001(void);
static void TPCS_SetActivePluginAsync_001(void);
//static void TPCS_ShutDown_IPC_001(void);

static void TPCSTestCases(void);


/**
 *  Plugin Control Service Test Cases END
 */

extern int TestCasesCount;
extern int Success;
extern int Failures;


int main(int argc, char **argv)
{
    TSCStartup();

    TPCSTestCases();
    TSCCleanup();
    dbus_shutdown();
    return 0;
}


static void TPCSTestCases(void)
{
#if 1
    TPCS_GetPluginInfo_001();
#endif
#if 1
    TPCS_GetPluginInfo_002();
    TPCS_GetPluginInfo_003();
    TPCS_GetPluginInfo_004();
    TPCS_GetPluginInfo_005();
    TPCS_GetPluginInfo_006();
    TPCS_GetPluginInfoSync_001();
	TPCS_GetPluginInfoAsync_001();
#endif
#if 1
    TPCS_InstallPlugin_001();
#endif
#if 1
    TPCS_InstallPlugin_002();
#endif
#if 1
    TPCS_InstallPlugin_003();
#endif
#if 1
    TPCS_InstallPlugin_004();
    TPCS_InstallPlugin_005();
#endif
#if 1
    TPCS_InstallPlugin_006();
    TPCS_InstallPlugin_007();
    TPCS_InstallPlugin_008();
    TPCS_InstallPlugin_009();
    TPCS_InstallPlugin_010();
    TPCS_InstallPlugin_011();
    TPCS_InstallPlugin_012();
    TPCS_InstallPlugin_013();
	TPCS_InstallPluginSync_001();
	TPCS_InstallPluginAsync_001();

#endif
#if 1
    TPCS_UninstallPlugin_001();
#endif
#if 1
    TPCS_UninstallPlugin_002();
#endif
#if 1
    TPCS_UninstallPlugin_003();
    TPCS_UninstallPlugin_004();
	TPCS_UninstallPluginSync_001();
	sleep(2);
	TPCS_UninstallPluginAsync_001();
	sleep(2);
#endif
#if 1
    TPCS_SetActivePlugin_001();
#endif
#if 1
    TPCS_SetActivePlugin_002();
    TPCS_SetActivePlugin_003();
	TPCS_SetActivePluginSync_001();
	sleep(2);
	TPCS_SetActivePluginAsync_001();
#endif
}

/**
 * Normal case, return config.xml
 */
static void TPCS_GetPluginInfo_001(void)
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();
    pid_t pidStub = 0;

    TESTCASECTOR(&TestCtx, __FUNCTION__);

    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_GETINFO_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], (const char*)CONFIG_TEST_NORMAL, rep_argc) == 0);
    xmlChar *data;
    int size;
    WriteToMemoryFromFile(&data, &size, CONFIG_FILE_W_PATH);
    TEST_ASSERT(strncmp((const char*)data, rep_argv[1], rep_argc) == 0);
    free((char*)data);

    TESTCASEDTOR(&TestCtx);
    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}
/**
 * Invalid config.xml, valid config.xml.new exists
 * recover corrupted config.xml from config.xml.new
 * return config.xml
 */
static void TPCS_GetPluginInfo_002(void)
{

    //prepare normal case, config.xml exists, config.xml.new exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        WriteToFileFromMemory(CONFIG_FILE_NEW, CONFIG_FILE_NEW_W_PATH);
    }

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_GETINFO_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], (const char*)CONFIG_FILE_NEW, rep_argc) == 0);


    FILE *pfConf = fopen(CONFIG_FILE_NEW_W_PATH, "r");
    TEST_ASSERT(pfConf == NULL);

    xmlChar *data;
    int size;
    WriteToMemoryFromFile(&data, &size, CONFIG_FILE_W_PATH);
    TEST_ASSERT(strncmp((const char*)data, rep_argv[1], rep_argc) == 0);
    free((char*)data);
    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
    return;
}

/**
 * Corrupted config.xml, no plugin service, return recovered config.xml, not plugin service
 */
static void TPCS_GetPluginInfo_003(void)
{
    //no config.xml.new exist, malformed config.xml
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_CORRUPTED, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_GETINFO_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], (const char*)CONFIG_DEFAULT_STRING, rep_argc) == 0);


    FILE *pfConf = fopen(CONFIG_FILE_NEW_W_PATH, "r");
    TEST_ASSERT(pfConf == NULL);

    xmlChar *data;
    int size;
    WriteToMemoryFromFile(&data, &size, CONFIG_FILE_W_PATH);
    TEST_ASSERT(strncmp((const char*)data, rep_argv[1], rep_argc) == 0);
    free((char*)data);
    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
    return;
}

/**
 * Malformed config.xml, return recovered config.xml
 */
static void TPCS_GetPluginInfo_004(void)
{
    //no config.xml.new exist, malformed config.xml
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_MALFORMED, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_GETINFO_PLUGIN,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], (const char*)CONFIG_DEFAULT_STRING, rep_argc) == 0);

    FILE *pfConf = fopen(CONFIG_FILE_NEW_W_PATH, "r");
    TEST_ASSERT(pfConf == NULL);

    xmlChar *data;
    int size;
    WriteToMemoryFromFile(&data, &size, CONFIG_FILE_W_PATH);
    TEST_ASSERT(strncmp((const char*)data, rep_argv[1], rep_argc) == 0);
    free((char*)data);
    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
    return;

}

/**
 * config.xml should return proposed default format, if not, return failure.
 */
static void TPCS_GetPluginInfo_005()
{
    // no config.xml.new exist, malformed config.xml
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
        FILE *fConf = fopen(CONFIG_FILE_W_PATH, "w");
        if (fConf)
        {
            fclose(fConf);
            status = remove(CONFIG_FILE_W_PATH);
            TEST_ASSERT(status == 0);
        }
    }

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_GETINFO_PLUGIN,
                              req_argc, req_argv, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], (const char*)CONFIG_DEFAULT_STRING, rep_argc) == 0);


    FILE *pfConf = fopen(CONFIG_FILE_NEW_W_PATH, "r");
    TEST_ASSERT(pfConf == NULL);

    xmlChar *data;
    int size;
    WriteToMemoryFromFile(&data, &size, CONFIG_FILE_W_PATH);
    TEST_ASSERT(strncmp((const char*)data, rep_argv[1], rep_argc) == 0);
    free((char*)data);
    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
    return;

}

/**
 * Corrupted config.xml, config.xml.new not exist, copy the test app to /opt/usr/app
 * recover corrupted config.xml by scan the installed apps, get list of plugins that have av category,
 * dynaically load each plugin library to get pluginInfo and update config.xml, check symbolic link to determine
 * active plugin
 * return config.xml
 */
static void TPCS_GetPluginInfo_006(void)
{
    //no config.xml.new exist, malformed config.xml
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
        FILE *fConf = fopen(CONFIG_FILE_W_PATH, "w");
        if (fConf)
        {
            fclose(fConf);
            status = remove(CONFIG_FILE_W_PATH);
            TEST_ASSERT(status == 0);
        }
    }
/*

    pid_t pidPlug = 0;
    pidPlug = AddPlugApp();

*/
    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 0;
    char *req_argv[0] = {};
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_GETINFO_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(strncmp(rep_argv[1], (const char*)CONFIG_DEFAULT_STRING, rep_argc) == 0);


    FILE *pfConf = fopen(CONFIG_FILE_NEW_W_PATH, "r");
    TEST_ASSERT(pfConf == NULL);

    xmlChar *data;

    int size;
    WriteToMemoryFromFile(&data, &size, CONFIG_FILE_W_PATH);
    TEST_ASSERT(strncmp((const char*)data, rep_argv[1], rep_argc) == 0);
    free((char*)data);
    TESTCASEDTOR(&TestCtx);

    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
    return;

}


static void TPCS_GetPluginInfoSync_001()
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;

    int i;

    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    hIpc = IpcClientOpen();


    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();
    sleep(1);
    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_GETINFO_PLUGIN);
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

static void TPCS_GetPluginInfoAsync_001()
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_GETINFO_PLUGIN);
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
 * normal case, install a plugin, return success, verify the symbolic link set up
 */
static void TPCS_InstallPlugin_001()
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_2);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_INSTALL_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_PLUGINS_PLUG, APP_ID_SAMPLE_2) == 0);
    TEST_ASSERT(HasNode(XPATH_ACTIVE_PLUGIN, APP_ID_SAMPLE_2) == 0);

    TESTCASEDTOR(&TestCtx);
    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}


/**
 * normal case, install two plugins name, verify name change
 */
static void TPCS_InstallPlugin_002()
{

    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args 1
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_1);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);
    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_INSTALL_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_PLUGINS_PLUG, APP_ID_SAMPLE_1) == 0);
    CleanupReply(&rep_argv, &rep_argc);


    //Request args 2
    int req_argc_1 = 1;
    char *req_argv_1[] = {};
    req_argv_1[0] = strdup((const char*) APP_ID_SAMPLE_2);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_INSTALL_PLUGIN,
                              req_argc_1, req_argv_1, &rep_argc, &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_PLUGINS_PLUG, APP_ID_SAMPLE_2) == 0);
    CleanupReply(&rep_argv, &rep_argc);


    int req_argc_2 = 1;
    char *req_argv_2[] = {};
    req_argv_2[0] = strdup((const char*) "IpcShutdown");

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_SHUTDOWN_IPC, req_argc_2,
                              req_argv_2, &rep_argc, &rep_argv, DEF_TIMEOUT);
    CleanupReply(&rep_argv, &rep_argc);

    TESTCASEDTOR(&TestCtx);
    // Cleanup - On both success and failure.

    IpcClientClose(hIpc);
    TerminateProcess(pidStub);
}

/**
 * normal case, upgrade a plugin version, verify version change
 */
static void TPCS_InstallPlugin_003()
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_1);
    int iResult = -1;

    //Request args 2
    int req_argc_1 = 1;
    char *req_argv_1[] = {};
    req_argv_1[0] = strdup((const char*) APP_ID_SAMPLE_1);

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_INSTALL_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_PLUGINS_PLUG, APP_ID_SAMPLE_1) == 0);

    system("exec rm -r /opt/usr/apps/u7097a278m/lib/plugin/*");
    system("exec cp  /opt/usr/apps/u7097a278m/lib/u709v4.2.1.so  /opt/usr/apps/u7097a278m/lib/plugin/");
    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_INSTALL_PLUGIN, req_argc_1, req_argv_1, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);

    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 2);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_PLUGINS_PLUG, APP_ID_SAMPLE_1) == 0);

    TESTCASEDTOR(&TestCtx);
    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);


}
/**
 * normal case, upgrade a plugin library, add one more library, verify library change
 */
static void TPCS_InstallPlugin_004()
{

}

/**
 * normal case, upgrade a plugin library, remove a library, verify library change
 */
static void TPCS_InstallPlugin_005()
{

}

/**
 * return failure, LIBRARY_NOT_FOUND, not change, shared library does not exist, did not change the existing setting
 */
static void TPCS_InstallPlugin_006()
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_NO_EXIST);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_INSTALL_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 1);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_FAILURE) == 0);

    TEST_ASSERT(HasNode(XPATH_PLUGINS_PLUG, APP_ID_NO_EXIST) == -1);

    TESTCASEDTOR(&TestCtx);
    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
    TerminateProcess(pidStub);

}


/**
 * return failure, NO_AV_CATEGORY, manifest.xml does not exist
 */
static void TPCS_InstallPlugin_007()
{

}

/**
 * return failure, FAILED_TO_LOAD_LIBRARY, does not contains APIs
 */
static void TPCS_InstallPlugin_008()
{

}

/**
 * return failure, INAVLID_XML, not contains
 */
static void TPCS_InstallPlugin_009()
{

}

/**
 * return failure, FAILED_TO_UNLOAD_LIBRARY
 */
static void TPCS_InstallPlugin_010()
{

}

/**
 * return failure, failed in insert xml to config.xml FAILED_UPDATE_CONFIG failed to replace its xml section, use the original one
 */
static void TPCS_InstallPlugin_011()
{

}

/**
 * return failure, failed in update config.xml FAILED_UPDATE_CONFIG failed to replace its xml section, use the original one
 */
static void TPCS_InstallPlugin_012()
{

}
/**
 * return failure, failed in switch the config.xml.new to config.xml FAILED_UPDATE_CONFIG failed to switch the config.xml, use the original one
 */
static void TPCS_InstallPlugin_013()
{

}


static void TPCS_InstallPluginSync_001()
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;

    int i;

    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    hIpc = IpcClientOpen();


    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();
    sleep(1);


    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_INSTALL_PLUGIN);
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

static void TPCS_InstallPluginAsync_001()
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_INSTALL_PLUGIN);
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
 * normal case, return success, remove the symbolic link as it is active plugin, update config.xml
 */
static void TPCS_UninstallPlugin_001()
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    FILE *fConf = fopen(CONFIG_FILE_W_PATH, "w");
    if (fConf)
    {
        fclose(fConf);
        int status = remove(CONFIG_FILE_W_PATH);
        TEST_ASSERT(status == 0);
    }
    //WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_1);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_UNINSTALL_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 1);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_PLUGINS_PLUG, APP_ID_SAMPLE_1) != 0);

    // Cleanup - On both success and failure.

    int req_argc_1 = 1;
    char *req_argv_1[] = {};
    req_argv_1[0] = strdup((const char*) "IpcShutdown");

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_SHUTDOWN_IPC, req_argc_1, req_argv_1, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);

    sleep(1);
    CleanupReply(&rep_argv, &rep_argc);
    TESTCASEDTOR(&TestCtx);

    IpcClientClose(hIpc);
    TerminateProcess(pidStub);

}

/**
 * normal case, return success, update config.xml
 */
static void TPCS_UninstallPlugin_002()
{

}


/**
 * failure, appid does not exist
 */
static void TPCS_UninstallPlugin_003()
{

}


/**
 * failure, similiar cases like library loading, xml, will apply later
 */
static void TPCS_UninstallPlugin_004()
{

}

static void TPCS_UninstallPluginSync_001(void)
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;

    int i;

    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    hIpc = IpcClientOpen();


    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();
    sleep(2);


    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_UNINSTALL_PLUGIN);
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
static void TPCS_UninstallPluginAsync_001(void)
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_UNINSTALL_PLUGIN);
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

static void TPCS_SetActivePluginSync_001(void)
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;

    int i;

    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    hIpc = IpcClientOpen();


    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();
    sleep(2);


    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_SETACTIVE_PLUGIN);
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

static void TPCS_SetActivePluginAsync_001(void)
{
#if defined(TIMES)
#undef TIMES
#endif

#define TIMES 10

    TestCase TestCtx;
    pid_t pidStub = 0;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;
    int i;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    ConTestContext ConCtxs[TIMES];
    pthread_t Threads[TIMES];
    MethodCall methods[TIMES];

    for (i = 0; i < TIMES; i++)
    {
        methods[i].szMethod = strdup(TPCS_METHOD_SETACTIVE_PLUGIN);
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

/*
static void TPCS_ShutDown_IPC_001()
{
    return;
    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) IPC_SHUTDOWN);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_SHUTDOWN_IPC, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 1);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);


    TESTCASEDTOR(&TestCtx);
    // Cleanup - On both success and failure.
    CleanupReply(&rep_argv, &rep_argc);
    IpcClientClose(hIpc);
}
*/

/**
 * normal case, appId, update config.xml, verify the symbolic link with the new one
 */
static void TPCS_SetActivePlugin_001()
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
//    WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_SAMPLE_1);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_SETACTIVE_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 1);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_ACTIVE_PLUGIN, APP_ID_SAMPLE_1) == 0);


    int req_argc_1 = 1;
    char *req_argv_1[] = {};
    req_argv_1[0] = strdup((const char*) "IpcShutdown");

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_SHUTDOWN_IPC, req_argc_1, req_argv_1, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    CleanupReply(&rep_argv, &rep_argc);
    sleep(1);
    IpcClientClose(hIpc);

    TESTCASEDTOR(&TestCtx);


    //TerminateProcess(pidStub);

}

/**
 * failure case, cannot found the active appId
 */
static void TPCS_SetActivePlugin_002()
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_NO_EXIST);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_SETACTIVE_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 1);
    TEST_ASSERT(strcmp(rep_argv[0], RETURN_FAILURE) == 0);
    TEST_ASSERT(HasNode(XPATH_ACTIVE_PLUGIN, ACTIVE_NONE) == 0);


    int req_argc_1 = 1;
    char *req_argv_1[] = {};
    req_argv_1[0] = strdup((const char*) "IpcShutdown");

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_SHUTDOWN_IPC, req_argc_1, req_argv_1, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    CleanupReply(&rep_argv, &rep_argc);
    sleep(1);
    IpcClientClose(hIpc);

    TESTCASEDTOR(&TestCtx);


    TerminateProcess(pidStub);
}

/**
 * if passing is null, should disable the active plugin
 */
static void TPCS_SetActivePlugin_003()
{
    //prepare normal case, config.xml exists, config.xml.new not exist.
    FILE *fNew = fopen(CONFIG_FILE_NEW_W_PATH, "w");
    if (fNew)
    {
        fclose(fNew);
        int status = remove(CONFIG_FILE_NEW_W_PATH);
        TEST_ASSERT(status == 0);
    }
    //WriteToFileFromMemory(CONFIG_TEST_NORMAL, CONFIG_FILE_W_PATH);

    pid_t pidStub = 0;

    TestCase TestCtx;
    TSC_IPC_HANDLE hIpc = INVALID_IPC_HANDLE;

    //Request args
    int req_argc = 1;
    char *req_argv[] = {};
    req_argv[0] = strdup((const char*) APP_ID_NULL);
    int iResult = -1;

    //Response argc
    int rep_argc = 0;
    char **rep_argv = NULL;

    hIpc = IpcClientOpen();

    TESTCASECTOR(&TestCtx, __FUNCTION__);
    pidStub = StartTPCSServerStub();

    TEST_ASSERT(hIpc != INVALID_IPC_HANDLE);

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_METHOD_SETACTIVE_PLUGIN, req_argc, req_argv, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    TEST_ASSERT(iResult == 0);

    TEST_ASSERT(rep_argc == 1);

    TEST_ASSERT(strcmp(rep_argv[0], RETURN_SUCCESS) == 0);
    TEST_ASSERT(HasNode(XPATH_ACTIVE_PLUGIN, ACTIVE_NONE) == 0);


    int req_argc_1 = 1;
    char *req_argv_1[] = {};
    req_argv_1[0] = strdup((const char*) "IpcShutdown");

    iResult = TSCSendMessageN(hIpc, TSC_DBUS_SERVER_PLUGIN_CHANNEL, TPCS_SHUTDOWN_IPC, req_argc_1, req_argv_1, &rep_argc,
                              &rep_argv, DEF_TIMEOUT);
    CleanupReply(&rep_argv, &rep_argc);


    sleep(1);
    IpcClientClose(hIpc);

    TESTCASEDTOR(&TestCtx);


    TerminateProcess(pidStub);

}


/**
 * Multi-Thread Test case: [Async + Sync methods] Verify if >1 Asynchronous method call succeeds in
 * every thread, when multiple threads are running asynchronous calls.
 */

/**
 * Multi-Thread Test case: [Async + Sync methods] Verify if Cancellation of previous Asynchronous
 * method call succeeds in every thread, when multiple threads are running asynchronous calls.
 */

static void TSCStartup(void)
{
    extern int TestCasesCount;
    extern int Success;
    extern int Failures;

    TestCasesCount = 0;
    Success = 0;
    Failures = 0;
}


static void TSCCleanup(void)
{
    LOG_OUT("@@@@@@@@@@@@@@@@@@@@@@@@\n");
    LOG_OUT("Test done: %d executed, %d passed, %d failure\n", TestCasesCount, Success, Failures);
}



static void WriteToFileFromMemory(const char *data, const char *file_name_w_path)
{
    FILE *pFile = fopen(file_name_w_path, "w");
    if (pFile)
    {
        //fprintf(stderr, "sizeof data: %d, write tofile :%s\n data:%s\n", sizeof(*data), file_name_w_path, data);
        fwrite(data, strlen(data), 1, pFile);
        fclose(pFile);
    }
}

static void WriteToMemoryFromFile(xmlChar **data, int *size, const char *file_name_w_path)
{
    xmlDoc *pXmlDoc = xmlParseFile(file_name_w_path);
    xmlDocDumpFormatMemory(pXmlDoc, data, size, 0);
    xmlFreeDoc(pXmlDoc);
}
