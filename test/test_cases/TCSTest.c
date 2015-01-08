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
#include <dlfcn.h>
#include "TCSImpl.h"
#include "TCSErrorCodes.h"

#include "TCSTest.h"


/* Test cases. */
static void TCSStartup(void);
static void TCSCleanup(void);
static void DumpVersions(void);
static void TCSLibraryOpen_0001(void);
static void TCSLibraryOpen_0002(void);
static void TCSLibraryOpen_0003(void);
static void TCSLibraryOpen_0004(void);
static void TCSGetLastError_0001(void);
static void TCSLibraryClose_0001(void);

static void TCSScanData_0001(void);
static void TCSScanData_0002(void);
static void TCSScanData_0003(void);
static void TCSScanData_0004(void);
static void TCSScanData_0005(void);
static void TCSScanData_0006(void);
static void TCSScanData_0007(void);
static void TCSScanData_0008(void);
static void TCSScanData_0009(void);
static void TCSScanData_0010(void);
static void TCSScanData_0011(void);
static void TCSScanData_0012(void);
static void TCSScanData_0013(void);
static void TCSScanData_0014(void);
static void TCSScanData_0015(void);
static void TCSScanData_0016(void);
static void TCSScanData_0017(void);
static void TCSScanData_0018(void);
static void TCSScanData_0019(void);
static void TCSScanData_0020(void);
static void TCSScanData_0021(void);
static void TCSScanData_0022(void);
static void TCSScanData_0023(void);
static void TCSScanData_0024(void);
static void TCSScanData_0025(void);
static void TCSScanData_0026(void);
static void TCSScanData_0027(void);
static void TCSScanData_0028(void);
static void TCSScanData_0029(void);
static void TCSScanData_0030(void);
static void TCSScanData_0031(void);
static void TCSScanData_0032(void);
static void TCSScanData_0033(void);
static void TCSScanData_0034(void);
static void TCSScanData_0035(void);
static void TCSScanData_0036(void);
static void TCSScanData_0037(void);
static void TCSScanData_0038(void);
static void TCSScanData_0039(void);
static void TCSScanData_0040(void);
static void TCSScanData_0041(void);
static void TCSScanData_0042(void);
static void TCSScanData_0043(void);
static void TCSScanData_0044(void);
static void TCSScanData_0045(void);
static void TCSScanData_0046(void);
static void TCSScanData_0047(void);
static void TCSScanData_0048(void);
static void TCSScanData_0049(void);
static void TCSScanData_0050(void);
static void TCSScanData_0051(void);
static void TCSScanData_0052(void);
static void TCSScanDataAsync_0053(void);
static void TCSScanDataAsync_0054(void);

static void TCSScanFile_0001(void);
static void TCSScanFile_0002(void);
static void TCSScanFile_0003(void);
static void TCSScanFile_0004(void);
static void TCSScanFile_0005(void);
static void TCSScanFile_0006(void);
static void TCSScanFile_0007(void);
static void TCSScanFile_0008(void);
static void TCSScanFile_0009(void);
static void TCSScanFile_0010(void);
static void TCSScanFile_0011(void);
static void TCSScanFile_0012(void);
static void TCSScanFile_0013(void);
static void TCSScanFile_0014(void);
static void TCSScanFile_0015(void);
static void TCSScanFile_0016(void);
static void TCSScanFile_0017(void);
static void TCSScanFile_0018(void);
static void TCSScanFile_0019(void);
static void TCSScanFile_0020(void);
static void TCSScanFile_0021(void);
static void TCSScanFile_0022(void);
static void TCSScanFile_0023(void);
static void TCSScanFile_0024(void);
static void TCSScanFile_0025(void);
static void TCSScanFile_0026(void);
static void TCSScanFile_0027(void);
static void TCSScanFile_0028(void);
static void TCSScanFile_0029(void);
static void TCSScanFile_0030(void);
static void TCSScanFile_0031(void);
static void TCSScanFile_0032(void);
static void TCSScanFile_0033(void);
static void TCSScanFile_0034(void);
static void TCSScanFile_0035(void);
static void TCSScanFileEx_0036(void);
static void TCSScanFileAsync_0037(void);
static void TCSScanFileAsync_0038(void);

static void TCSGetVersion_0001(void);
static void TCSGetVersion_0002(void);
static void TCSGetVersion_0003(void);

static void TCSGetInfo_0001(void);
static void TCSGetInfo_0002(void);
static void TCSGetInfo_0003(void);

static void TestCases(void);


extern int TestCasesCount;
extern int Success;
extern int Failures;


int main(int argc, char **argv)
{

    TCSStartup();
    TestCases();
    TCSCleanup();

    return 0;
}


static void TestCases(void)
{
    TCSLibraryOpen_0001();
    TCSLibraryOpen_0002();
    TCSLibraryOpen_0003();
    TCSLibraryOpen_0004();

    TCSGetLastError_0001();

    TCSLibraryClose_0001();

    TCSScanData_0001();
    TCSScanData_0002();
    TCSScanData_0003();
    TCSScanData_0004();
    TCSScanData_0005();
    TCSScanData_0006();
    TCSScanData_0007();
    TCSScanData_0008();
    TCSScanData_0009();
    TCSScanData_0010();
    TCSScanData_0011();
    TCSScanData_0012();
    TCSScanData_0013();
    TCSScanData_0014();
    TCSScanData_0015();
    TCSScanData_0016();
    TCSScanData_0017();
    TCSScanData_0018();
    TCSScanData_0019();
    TCSScanData_0020();
    TCSScanData_0021();
    TCSScanData_0022();
    TCSScanData_0023();
    TCSScanData_0024();
    TCSScanData_0025();
    TCSScanData_0026();
    TCSScanData_0027();
    TCSScanData_0028();
    TCSScanData_0029();
    TCSScanData_0030();
    TCSScanData_0031();
    TCSScanData_0032();
    TCSScanData_0033();
    TCSScanData_0034();
    TCSScanData_0035();
    TCSScanData_0036();
    TCSScanData_0037();
    TCSScanData_0038();
    TCSScanData_0039();
    TCSScanData_0040();
    TCSScanData_0041();
    TCSScanData_0042();
    TCSScanData_0043();
    TCSScanData_0044();
    TCSScanData_0045();
    TCSScanData_0046();
    TCSScanData_0047();
    TCSScanData_0048();
    TCSScanData_0049();
    TCSScanData_0050();
    TCSScanData_0051();
    TCSScanData_0052();
    TCSScanDataAsync_0053();
    TCSScanDataAsync_0054();

    TCSScanFile_0001();
    TCSScanFile_0002();
    TCSScanFile_0003();
    TCSScanFile_0004();
    TCSScanFile_0005();
    TCSScanFile_0006();
    TCSScanFile_0007();
    TCSScanFile_0008();
    TCSScanFile_0009();
    TCSScanFile_0010();
    TCSScanFile_0011();
    TCSScanFile_0012();
    TCSScanFile_0013();
    TCSScanFile_0014();
    TCSScanFile_0015();
    TCSScanFile_0016();
    TCSScanFile_0017();
    TCSScanFile_0018();
    TCSScanFile_0019();
    TCSScanFile_0020();
    TCSScanFile_0021();
    TCSScanFile_0022();
    TCSScanFile_0023();
    TCSScanFile_0024();
    TCSScanFile_0025();
    TCSScanFile_0026();
    TCSScanFile_0027();
    TCSScanFile_0028();
    TCSScanFile_0029();
    TCSScanFile_0030();
    TCSScanFile_0031();
    TCSScanFile_0032();
    TCSScanFile_0033();
    TCSScanFile_0034();
    TCSScanFile_0035();
    TCSScanFileEx_0036();
    TCSScanFileAsync_0037();
    TCSScanFileAsync_0038();

    TCSGetVersion_0001();
    TCSGetVersion_0002();
    TCSGetVersion_0003();

    TCSGetInfo_0001();
    TCSGetInfo_0002();
    TCSGetInfo_0003();
}


static void TCSLibraryOpen_0001(void)
{
    TestCase TestCtx;
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE);
    TESTCASEDTOR(&TestCtx);
    TCSLibraryClose(hLib);
}


static void TCSLibraryOpen_0002(void)
{
    int iErr;
    TestCase TestCtx;
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);

    BackupEngine();
    system("rm -f /opt/usr/share/sec_plugin/libengine.so");

    TEST_ASSERT((hLib = TCSLibraryOpen()) == INVALID_TCSLIB_HANDLE);
    iErr = TCSGetLastError(hLib);
    TEST_ASSERT(TCS_ERRMODULE(iErr) == TCS_ERROR_MODULE_GENERIC);
    TEST_ASSERT(TCS_ERRCODE(iErr) == TCS_ERROR_NOT_IMPLEMENTED);
    TESTCASEDTOR(&TestCtx);

    RestoreEngine();
}


static void TCSGetLastError_0001(void)
{
    int iErr;
    TestCase TestCtx;

    BackupEngine();
    system("rm -f /opt/usr/share/sec_plugin/libengine.so");

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    iErr = TCSGetLastError(INVALID_TCSLIB_HANDLE);
    TEST_ASSERT(TCS_ERRMODULE(iErr) == TCS_ERROR_MODULE_GENERIC);
    TEST_ASSERT(TCS_ERRCODE(iErr) == TCS_ERROR_NOT_IMPLEMENTED);
    TESTCASEDTOR(&TestCtx);

    RestoreEngine();
}


static void TCSScanData_0052(void)
{
    TestCase TestCtx;
    TCSScanParam SP = {0};
    TCSScanResult SR= {0};

    BackupEngine();
    system("rm -f /opt/usr/share/sec_plugin/libengine.so");

    SP.iAction = TCS_SA_SCANONLY;
    SP.iDataType = TCS_DTYPE_UNKNOWN;
    SP.iCompressFlag = 1;
    SP.pPrivate = NULL;
    SP.pfGetSize = NULL;
    SP.pfSetSize = NULL;
    SP.pfRead = NULL;
    SP.pfWrite = NULL;
    SP.pfCallBack = NULL;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT(TCSScanData(INVALID_TCSLIB_HANDLE, &SP, &SR) == -1);
    TESTCASEDTOR(&TestCtx);

    RestoreEngine();
}


static void TCSScanFile_0034(void)
{
    TestCase TestCtx;
    TCSScanResult SR = {0};

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT(TCSScanFile(INVALID_TCSLIB_HANDLE, "file",
                            TCS_DTYPE_UNKNOWN, TCS_SA_SCANONLY, 1, &SR) == -1);
    TESTCASEDTOR(&TestCtx);
}


static void TCSLibraryClose_0001(void)
{
    TestCase TestCtx;
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE)
    TEST_ASSERT(TCSLibraryClose(hLib) == 0);
    TESTCASEDTOR(&TestCtx);
}


static void TCSScanData_0001(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_BUFFER, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0002(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_BUFFER, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0003(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0004(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0005(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_HTML, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0006(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_HTML, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0007(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_HTML, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0008(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_HTML, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0009(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_URL, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0010(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_URL, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0011(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_URL, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0012(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_URL, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0013(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_EMAIL, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0014(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_EMAIL, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0015(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_EMAIL, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0016(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_EMAIL, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0017(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_PHONE, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0018(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_PHONE, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0019(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_PHONE, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0020(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_PHONE, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0021(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVA, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0022(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVA, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0023(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVA, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0024(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVA, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0025(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVAS, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0026(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVAS, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0027(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVAS, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0028(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVAS, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0029(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_TEXT, BENIGN_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0030(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_TEXT, BENIGN_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0031(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_TEXT, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0032(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_TEXT, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0033(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_MULTIPLE, INFECTED_DATA,
                 TCS_SA_SCANONLY, NULL);
}


static void TCSScanData_0034(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_MULTIPLE, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbScanCallback);
}


static void TCSScanData_0035(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0036(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_HTML, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0037(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_URL, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0038(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_EMAIL, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0039(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_PHONE, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0040(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_JAVA, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0041(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_TEXT, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0042(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_MULTIPLE, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0043(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA,
                 TCS_SA_SCANONLY, &CbAbortCallback);
}


static void TCSScanData_0044(void)
{

    TestScanData(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA,
                 TCS_SA_SCANREPAIR, &CbScanCallback);
}


static void TCSScanData_0045(void)
{
    TestCase TestCtx;
    TCSScanParam SP = {0};
    TCSScanResult SR= {0};

    SP.iAction = TCS_SA_SCANONLY;
    SP.iDataType = TCS_DTYPE_UNKNOWN;
    SP.iCompressFlag = 1;
    SP.pPrivate = NULL;
    SP.pfGetSize = NULL;
    SP.pfSetSize = NULL;
    SP.pfRead = NULL;
    SP.pfWrite = NULL;
    SP.pfCallBack = NULL;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT(TCSScanData(INVALID_TCSLIB_HANDLE, &SP, &SR) == -1);
    TESTCASEDTOR(&TestCtx);
}


static void TCSScanData_0046(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanData(&TestCtx, TCS_SA_SCANONLY);
    TESTCASEDTOR(&TestCtx);
}


static void TCSScanData_0047(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanData(&TestCtx, TCS_SA_SCANREPAIR);
    TESTCASEDTOR(&TestCtx);
}


static void TCSScanData_0048(void)
{

    TestScanDataEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, BENIGN_DATA,
                   TCS_SA_SCANONLY, 1, &CbScanCallback);
}


static void TCSScanData_0049(void)
{

    TestScanDataEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, BENIGN_DATA,
                   TCS_SA_SCANONLY, 0, &CbScanCallback);
}


static void TCSScanData_0050(void)
{

    TestScanDataEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, INFECTED_DATA,
                   TCS_SA_SCANONLY, 1, &CbScanCallback);
}


static void TCSScanData_0051(void)
{

    TestScanDataEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, INFECTED_DATA,
                   TCS_SA_SCANONLY, 0, &CbScanCallback);
}

static void TCSScanDataAsync_0053(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanDataAsync(&TestCtx, TCS_SA_SCANONLY);
    TESTCASEDTOR(&TestCtx);
}

static void TCSScanDataAsync_0054(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanDataAsync(&TestCtx, ACTION_SCAN_CANCEL);
    TESTCASEDTOR(&TestCtx);
}

static void TCSScanFile_0001(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_BUFFER, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0002(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0003(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_HTML, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0004(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_HTML, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0005(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_URL, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0006(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_URL, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0007(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_EMAIL, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0008(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_EMAIL, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0009(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_PHONE, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0010(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_PHONE, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0011(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_JAVA, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0012(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_JAVA, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0013(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_TEXT, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0014(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_TEXT, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0015(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_MULTIPLE, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0016(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0017(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_HTML, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0018(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_URL, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0019(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_EMAIL, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0020(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_PHONE, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0021(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_JAVA, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0022(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_TEXT, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0023(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_MULTIPLE, INFECTED_DATA,
                 TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0024(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_BUFFER, INFECTED_DATA, TCS_SA_SCANREPAIR);
}


static void TCSScanFile_0025(void)
{
    TestCase TestCtx;
    TCSScanResult SR = {0};

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT(TCSScanFile(INVALID_TCSLIB_HANDLE, "file",
                            TCS_DTYPE_UNKNOWN, TCS_SA_SCANONLY, 1, &SR) == -1);
    TESTCASEDTOR(&TestCtx);
}


static void TCSScanFile_0026(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanFile(&TestCtx, TCS_SA_SCANONLY);
    TESTCASEDTOR(&TestCtx);
}


static void TCSScanFile_0027(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanFile(&TestCtx, TCS_SA_SCANREPAIR);
    TESTCASEDTOR(&TestCtx);
}


static void TCSScanFile_0028(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_JAVAS, BENIGN_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0029(void)
{

    TestScanFile(__FUNCTION__, MALWARE_TTYPE_JAVAS, INFECTED_DATA, TCS_SA_SCANONLY);
}


static void TCSScanFile_0030(void)
{

    TestScanFileEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, BENIGN_DATA, TCS_SA_SCANONLY, 1);
}


static void TCSScanFile_0031(void)
{

    TestScanFileEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, INFECTED_DATA, TCS_SA_SCANONLY, 1);
}


static void TCSScanFile_0032(void)
{

    TestScanFileEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, BENIGN_DATA, TCS_SA_SCANONLY, 0);
}


static void TCSScanFile_0033(void)
{

    TestScanFileEx(__FUNCTION__, MALWARE_TTYPE_COMPRESS, INFECTED_DATA, TCS_SA_SCANONLY, 0);
}


static void TCSScanFile_0035(void)
{

    TestScanFileEx(__FUNCTION__, MALWARE_TTYPE_MULTIPLE_BYTES, INFECTED_DATA, TCS_SA_SCANONLY, 1);
}

static void TCSScanFileEx_0036(void)
{
    TestScanFileExCancel(__FUNCTION__, MALWARE_TTYPE_COMPRESS, BENIGN_DATA, TCS_SA_SCANONLY);
}

static void TCSScanFileAsync_0037(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanFileAsync(&TestCtx, TCS_SA_SCANONLY);
    TESTCASEDTOR(&TestCtx);
}

static void TCSScanFileAsync_0038(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    ConScanFileAsync(&TestCtx, ACTION_SCAN_CANCEL);
    TESTCASEDTOR(&TestCtx);

}

static void TCSGetVersion_0001(void)
{

    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE);
    TCSVerInfo VerInfo;
    TEST_ASSERT(TCSGetVersion(hLib, &VerInfo) == 0);
    TEST_ASSERT(strlen(VerInfo.szPluginVer) > 0);
    TEST_ASSERT(strlen(VerInfo.szFrameworkVer) > 0);
    TEST_ASSERT(strncmp(VerInfo.szFrameworkVer, TCS_FRAMEWORK_VERSION, strlen(TCS_FRAMEWORK_VERSION)) == 0);
    TCSLibraryClose(hLib);
    TESTCASEDTOR(&TestCtx);
}

static void TCSGetVersion_0002(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TCSVerInfo VerInfo;
    TEST_ASSERT(TCSGetVersion(INVALID_TCSLIB_HANDLE, &VerInfo) == -1);
    TESTCASEDTOR(&TestCtx);
}

static void TCSGetVersion_0003(void)
{

    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE);
    TEST_ASSERT(TCSGetVersion(hLib, NULL) == -1);
    TCSLibraryClose(hLib);
    TESTCASEDTOR(&TestCtx);
}

static void TCSGetInfo_0001(void)
{
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE);
    char szMetaInfo[TCS_META_MAX];
    TEST_ASSERT(TCSGetInfo(hLib, szMetaInfo) == 0);
    TEST_ASSERT(strlen(szMetaInfo) > 0);
    TCSLibraryClose(hLib);
    TESTCASEDTOR(&TestCtx);
}

static void TCSGetInfo_0002(void)
{
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    char szMetaInfo[TCS_META_MAX];
    TEST_ASSERT(TCSGetInfo(INVALID_TCSLIB_HANDLE, szMetaInfo) == -1);
    TESTCASEDTOR(&TestCtx);
}

static void TCSGetInfo_0003(void)
{
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE);
    TEST_ASSERT(TCSGetInfo(hLib, NULL) == -1);
    TCSLibraryClose(hLib);
    TESTCASEDTOR(&TestCtx);
}

static void TCSStartup(void)
{
    extern int TestCasesCount;
    extern int Success;
    extern int Failures;

    TestCasesCount = 0;
    Success = 0;
    Failures = 0;
    CreateTestDirs();
}


static void TCSCleanup(void)
{
    LOG_OUT("@@@@@@@@@@@@@@@@@@@@@@@@\n");
    LOG_OUT("Test done: %d executed, %d passed, %d failure\n", TestCasesCount, Success, Failures);
    DestoryTestDirs();
    DumpVersions();
}


static void DumpVersions(void)
{
    /* Dump AV Plugin/Framework version */
    TCSLIB_HANDLE hLib = TCSLibraryOpen();
    TCSVerInfo VerInfo;
    if (TCSGetVersion(hLib, &VerInfo) == 0)
        LOG_OUT("Version CS Plugin|Framework -> %s|%s\n", VerInfo.szPluginVer, VerInfo.szFrameworkVer);
    else
        LOG_OUT("Version CS Plugin|Framework -> Not available\n");
    TCSLibraryClose(hLib);
}


static void TCSLibraryOpen_0003(void)
{
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);
    /* pre-condition is stub library */
    BackupEngine();
    system("rm -f /opt/usr/share/sec_plugin/libengine.so");

    TEST_ASSERT((hLib = TCSLibraryOpen()) == INVALID_TCSLIB_HANDLE);
    RestoreEngine();

    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE);
    TCSLibraryClose(hLib);
    TESTCASEDTOR(&TestCtx);
}


static void TCSLibraryOpen_0004(void)
{
    TCSLIB_HANDLE hLib = INVALID_TCSLIB_HANDLE;
    TestCase TestCtx;

    TESTCASECTOR(&TestCtx, __FUNCTION__, 0, 0, 0, NULL);

    TEST_ASSERT((hLib = TCSLibraryOpen()) != INVALID_TCSLIB_HANDLE);

    BackupEngine();
    system("rm -f /opt/usr/share/sec_plugin/libengine.so");
    TCSLibraryClose(hLib);

    TEST_ASSERT((hLib = TCSLibraryOpen()) == INVALID_TCSLIB_HANDLE);

    RestoreEngine();
    TESTCASEDTOR(&TestCtx);
}

