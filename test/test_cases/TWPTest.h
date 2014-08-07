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

#ifndef TWPTEST_H
#define TWPTEST_H


#include <setjmp.h>


#ifdef __cplusplus 
extern "C" {
#endif

#define TEST_SUITE_VERSION "0.0.1"

/* Immediate value definitions. */
#define MAX_TEST_NUM 128

/* Maximum WP API name length. */
#define MAX_TWP_API_NAME_LEN 128

/* Output methods. */
#define LOG_OUT(fmt, x...) printf("Log:"fmt, ##x)

#define TRY_TEST { \
    TestCasesCount++; \
    int _ret_ = setjmp(WPJmpBuf); \
    if (_ret_ == 1) { \
        Failures++; \
    } else { \

#define FAIL_TEST longjmp(WPJmpBuf, 1);

#define TESTCASECTOR(_ctx_, _api_) \
        TRY_TEST \
        TestCaseCtor(_ctx_, _api_);

#define TESTCASEDTOR(_ctx_) \
        TestCaseDtor(_ctx_); \
    } \
} \

/* Test assert method. */
/* Macro needs TestCtx in scope. */
#define TEST_ASSERT(cond) \
        if (!(cond)) \
        { \
            LOG_OUT("Test failed!! : %s at %s:%d\n", TestCtx.szAPIName, __FILE__, __LINE__); \
            FAIL_TEST \
        } \

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
    char szAPIName[MAX_TWP_API_NAME_LEN]; /* TWP API names */
} TestCase;

/**
 * Test request data structure
 */
typedef struct TRequest
{
    TWPRequest Request;
    const char *pszUrl;
    size_t ResponseLength;
    char *ResponseBody;
    XM_HTTP_HANDLE hHttp;
    char *pData;
    unsigned int uLength;
    unsigned int uRead;
    size_t ResponseBytesRead;
} TRequest;


/*
 * Very simple/thin porting layer
 */

/* Test framework */
extern void TestCaseCtor(TestCase *pCtx, const char *pszAPI);
extern void TestCaseDtor(TestCase *pCtx);
extern void RestoreEngine();
extern void RemoveEngine();
extern void BackupEngine();

extern TWP_RESULT CbSend(struct TWPRequest *pRequest, TWPResponseHandle hResponse,
                         const void *pData, unsigned int uLength);
extern TWP_RESULT CbRecv(struct TWPRequest *pRequest, void *pBuffer, unsigned int uBufferLength,
                         unsigned int *puLength);
extern TWP_RESULT CbSetUrl(struct TWPRequest *pRequest, const char *pszUrl, unsigned int uLength);
extern TWP_RESULT CbSetMethod(struct TWPRequest *pRequest, TWPSubmitMethod Method);
extern long GenerateRandomNumber();
extern void DestoryTestDirs(void);
extern int CreateTestDirs(void);

extern jmp_buf WPJmpBuf;

#ifdef __cplusplus 
}
#endif

#endif /* TWPTEST_H */
