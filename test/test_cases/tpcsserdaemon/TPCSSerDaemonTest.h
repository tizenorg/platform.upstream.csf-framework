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

#ifndef TSCTEST_H
#define TSCTEST_H

#include <setjmp.h>
#include "Debug.h"
#include "IpcClient.h"
#include "IpcServer.h"
#include "IpcTypes.h"

#ifdef __cplusplus 
extern "C" {
#endif

#define TEST_SUITE_VERSION "0.0.1"

/* Immediate value definitions. */
#define MAX_TEST_NUM 128

/* Maximum SC API name length. */
#define MAX_TSC_API_NAME_LEN 128

/* Default maximum number of threads for concurrency test. */
#define MAX_TEST_THREADS 10

/* Default maximum concurrency test timeout (in seconds). */
#define DEFAULT_CONCURRENCY_TEST_TIMEOUT 30

/* Sleep interval for thread context switch. */
#define SLEEP_INTERVAL 500

/* Output methods. */
#define LOG_OUT(fmt, x...) printf("Log:"fmt, ##x)

#define TRY_TEST { \
    TestCasesCount++; \
    int _ret_ = setjmp(SCJmpBuf); \
    if (_ret_ == 1) { \
        Failures++; \
    } else { \

#define FAIL_TEST longjmp(SCJmpBuf, 1);

#define TESTCASECTOR(_ctx_, _api_) \
        TRY_TEST \
        TestCaseCtor(_ctx_, _api_);

#define TESTCASEDTOR(_ctx_) \
        TestCaseDtor(_ctx_); \
    } \
} \

/* Test assert method. */
#define TEST_ASSERT(cond)                                                       \
    if (!(cond))                                                                \
    {                                                                           \
        LOG_OUT("Test failed!! at : %s, %d\n", __FILE__, __LINE__);             \
        FAIL_TEST                                                               \
    }

#define TEST_ASSERT_LOG(cond, log)                                              \
    if (!(cond))                                                                \
    {                                                                           \
        LOG_OUT("%s\n", log);                                                   \
        LOG_OUT("Test failed!! at : %s, %d\n", __FILE__, __LINE__);             \
        FAIL_TEST                                                               \
    }

#define ELEMENT_NUM(ary) (sizeof(ary) / sizeof((ary)[0]))

/* Content directory for testing. */
#define TSC_TEST_CONTENT_DIR "contents_test"

/* Content backup directory. */
#define TSC_BACKUP_CONTENT_DIR "contents_bak"

/**
 * Test case information data
 */
typedef struct TestCase_struct
{
    char szAPIName[MAX_TSC_API_NAME_LEN]; /* TSC API names */
} TestCase;


/**
 * Concurrency test data
 */
typedef struct ConTestContext_struct
{
    TestCase *pTestCtx;
    const char *szMethod; /* Server method to be called inside thread. */
    TSC_IPC_HANDLE hIpc;
    int timeout;
    int iCid; /* Concurrency test id. */
    TSCCallback cbReply;

    /* Report concurrency test status. 1 - success, -1 - failure, 0 - running. */
    int iConTestRet;    /* Return value from Concurrent thread, which makes async call */
    int iConTestReplyRet; /* Return value from Asynchronous callback. */
} ConTestContext;


typedef struct MethodCall_struct
{
    char *szMethod;
    int isAsync;  /* 0 = Synchronous; 1 = Asynchronous ; 2 = Cancel*/
    TSCCallback pfnCallback;
} MethodCall;



/**
 * sample app id
 */
#define APP_ID_SAMPLE_1 "u7097a278m"
#define APP_ID_SAMPLE_2 "n7097a278m"
#define APP_ID_SAMPLE_3 "q7097a278m"
#define IPC_SHUTDOWN "SHUTDOWN"
#define APP_ID_NO_EXIST "noexist"
#define XPATH_PLUGINS_PLUG "//TPCSConfig/Plugins/Plug/AppId"

/* Method names supported by the TPCS Server Stub */
#define TPCS_METHOD_GETINFO_PLUGIN "TPCSGetInfoPlugin"
#define TPCS_METHOD_INSTALL_PLUGIN "TPCSInstallPlugin"
#define TPCS_METHOD_SETACTIVE_PLUGIN "TPCSSetActivePlugin"
#define TPCS_METHOD_UNINSTALL_PLUGIN "TPCSUninstallPlugin"

#define TPCS_SHUTDOWN_IPC "IpcShutdown"

/* Macros for TPCS method test */
// this is different from CONFIG_DEFAULT_STRING in TPCSServerStub.h
#define CONFIG_TEST_NORMAL "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
    <!DOCTYPE TPCSConfig SYSTEM \"tpcs_config.dtd\">\n\
    <TPCSConfig>\n\
    <AppPaths>\n\
    <Path>/opt/usr/apps</Path>\n\
    <Path>/sdcard</Path>\n \
    </AppPaths>\n\
    <Active>\n\
        <AppId>None</AppId>\n\
    </Active>\n\
    <Plugins>\n\
    </Plugins>\n\
</TPCSConfig>"

#define CONFIG_CORRUPTED "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
        <!DOCTYPE TPCSConfig SYSTEM \"tpcs_config.dtd\">\n\
        <TPCSConfig>\n\
        <AppPaths>\n\
        <Path>/opt/usr/apps</Path>\n\
        <Path>/sdcard</Path>\n \
        </AppPaths>\n\
        <Active>\n\
            <AppId>None</AppId>\n\
        </Active>\n\
        <Plugins>\n\
        </Plugins>\n\
    "

#define CONFIG_MALFORMED "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
        <!DOCTYPE TPCSConfig SYSTEM \"tpcs_config.dtd\">\n\
        <TPCSConfig>\n\
        <AppPaths>\n\
        <Path>/opt/usr/apps</Path>\n\
        <Path>/sdcard</Path>\n \
        </AppPaths>\n\
        <Plugins>\n\
        </Plugins>\n\
    </TPCSConfig>"

#define CONFIG_FILE_NEW   "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
        <!DOCTYPE TPCSConfig SYSTEM \"tpcs_config.dtd\">\n\
        <TPCSConfig>\n\
        <AppPaths>\n\
        <Path>/opt/usr/apps</Path>\n\
        <Path>/sdcard</Path>\n \
        </AppPaths>\n\
        <Active>\n\
            <AppId>Emasdf</AppId>\n\
        </Active>\n\
        <Plugins>\n\
        </Plugins>\n\
    </TPCSConfig>"

#define CONFIG_FILE_NEW_CORRUPTTED   "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
        <!DOCTYPE TPCSConfig SYSTEM \"tpcs_config.dtd\">\n\
        <TPCSConfig>\n\
        <AppPaths>\n\
        <Path>/opt/usr/apps</Path>\n\
        <Path>/sdcard</Path>\n \
        </AppPaths>\n\
        <Active>\n\
            <AppId>Emasdf</AppId>\n\
        </Active>\n\
        <Plugins>\n\
        </Plugins>\n\
    </TPCSConfig"

/*
 * Very simple/thin porting layer
 */

/* Test framework */
extern void TestCaseCtor(TestCase *pCtx, const char *pszAPI);
extern void TestCaseDtor(TestCase *pCtx);


extern jmp_buf SCJmpBuf;




#ifdef __cplusplus 
}
#endif

#endif /* TSCTEST_H */
