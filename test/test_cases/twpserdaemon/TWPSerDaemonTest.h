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

#ifndef TWPSERDAEMONTEST_H
#define TWPSERDAEMONTEST_H

#define TWPGETVERSIONMETHOD "TWPSerGetVersion"
#define TWPGETURLREPUTATIONMETHOD "TWPSerGetURLReputation"

#include <setjmp.h>
#include "IpcClient.h"

#define Test_TWP_Minimal "0"
#define Test_TWP_Unverified "1"
#define Test_TWP_Medium "2"
#define Test_TWP_High "3"

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

#if defined(DEBUG)
#define DEBUG_LOG(_fmt_, _param_...) { \
                                        FILE *fp = fopen("/tmp/twpserdaemontestlog.txt", "a"); \
                                        if (fp != NULL) \
                                        { \
                                            printf("%s,%d: " _fmt_, __FILE__, __LINE__, ##_param_); \
                                            fprintf(fp, "%s,%d: " _fmt_, __FILE__, __LINE__, ##_param_); \
                                            fclose(fp); \
                                        } \
                                       }
#else
#define DEBUG_LOG(_fmt_, _param_...)
#endif

#define TRY_TEST { \
    TestCasesCount++; \
    int _ret_ = setjmp(SCJmpBuf); \
    if (_ret_ == 1) { \
        Failures++; \
        LOG_OUT("@@@@@@@@@@End Test: %s @@@@@@@@@@@@@@\n\n\n", TestCtx.szAPIName);\
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
        LOG_OUT("Test failed!! : %s at %s:%d\n", TestCtx.szAPIName, __FILE__, __LINE__);\
        FAIL_TEST                                                               \
    }

#define TEST_ASSERT_LOG(cond, log)                                              \
    if (!(cond))                                                                \
    {                                                                           \
        LOG_OUT("%s\n", log);                                                   \
        LOG_OUT("Test failed!! : %s at %s:%d\n\n\n", TestCtx.szAPIName, __FILE__, __LINE__);\
        FAIL_TEST                                                               \
    }

#define ELEMENT_NUM(ary) (sizeof(ary) / sizeof((ary)[0]))

/* Content directory for testing. */
#define TWP_TEST_CONTENT_DIR "contents_test"

/* Content backup directory. */
#define TWP_BACKUP_CONTENT_DIR "contents_bak"

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
    IpcClientInfo *pInfo;
    int timeout;
    int iCid; /* Concurrency test id. */

    /* Report concurrency test status. 1 - success, -1 - failure, 0 - running. */
    int iConTestRet;    /* Return value from Concurrent thread, which makes async call */
    int iConTestReplyRet; /* Return value from Asynchronous callback. */
} ConTestContext;


typedef struct MethodCall_struct
{
    const char *szMethod;
    int isAsync;  /* 0 = Synchronous; 1 = Asynchronous */
} MethodCall;

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

#endif /* TWPSERDAEMONTEST_H */
