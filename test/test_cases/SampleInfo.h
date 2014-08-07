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

#ifndef SAMPLE_INFO_H
#define SAMPLE_INFO_H

#ifdef __cplusplus 
extern "C" {
#endif

#define TCS_MAX_MALWARE_NAME_LEN 64

#define MALWARE_1_0_0 "Malware-fortest-1.0.0"
#define MALWARE_1_1_0 "Malware-fortest-1.1.0"
#define MALWARE_1_2_0 "Malware-fortest-1.2.0"
#define MALWARE_1_3_0 "Malware-fortest-1.3.0"
#define MALWARE_1_4_0 "Malware-fortest-1.4.0"
#define MALWARE_1_5_0 "Malware-fortest-1.5.0"
#define MALWARE_1_6_0 "Malware-fortest-1.6.0"
#define MALWARE_1_7_0 "Malware-fortest-1.7.0"
#define MALWARE_1_8_0 "Malware-fortest-1.8.0"
#define MALWARE_1_9_0 "Malware-fortest-1.9.0"

#define VARIANT_1_0_0 "Variant-fortest-1.0.0"
#define VARIANT_1_1_0 "Variant-fortest-1.1.0"
#define VARIANT_1_2_0 "Variant-fortest-1.2.0"
#define VARIANT_1_3_0 "Variant-fortest-1.3.0"
#define VARIANT_1_4_0 "Variant-fortest-1.4.0"
#define VARIANT_1_5_0 "Variant-fortest-1.5.0"
#define VARIANT_1_6_0 "Variant-fortest-1.6.0"
#define VARIANT_1_7_0 "Variant-fortest-1.7.0"
#define VARIANT_1_8_0 "Variant-fortest-1.8.0"
#define VARIANT_1_9_0 "Variant-fortest-1.9.0"

#define BUFFER_MALWARE_NAME MALWARE_1_6_0
#define BUFFER_VARIANT_NAME VARIANT_1_6_0
#define BUFFER_SEVERITY_CLASS TCS_SC_USER
#define BUFFER_BEHAVIOR_CLASS TCS_BC_LEVEL1
#define BUFFER_MALWARE_TYPE TCS_VTYPE_MALWARE
#define BUFFER_FILE_NAME_1 "tcs-testfile-1.buf"
#define BUFFER_FILE_NAME_0 "tcs-testfile-0.buf"

#define HTML_MALWARE_NAME MALWARE_1_0_0
#define HTML_VARIANT_NAME VARIANT_1_0_0
#define HTML_SEVERITY_CLASS TCS_SC_USER
#define HTML_BEHAVIOR_CLASS TCS_BC_LEVEL0
#define HTML_MALWARE_TYPE TCS_VTYPE_MALWARE
#define HTML_FILE_NAME_1 "tcs-testfile-1.html"
#define HTML_FILE_NAME_0 "tcs-testfile-0.html"

#define URL_MALWARE_NAME MALWARE_1_1_0
#define URL_VARIANT_NAME VARIANT_1_1_0
#define URL_SEVERITY_CLASS TCS_SC_USER
#define URL_BEHAVIOR_CLASS TCS_BC_LEVEL1
#define URL_MALWARE_TYPE TCS_VTYPE_MALWARE
#define URL_FILE_NAME_1 "tcs-testfile-1.url"
#define URL_FILE_NAME_0 "tcs-testfile-0.url"

#define EMAIL_MALWARE_NAME MALWARE_1_2_0
#define EMAIL_VARIANT_NAME VARIANT_1_2_0
#define EMAIL_SEVERITY_CLASS TCS_SC_TERMINAL
#define EMAIL_BEHAVIOR_CLASS TCS_BC_LEVEL2
#define EMAIL_MALWARE_TYPE TCS_VTYPE_MALWARE
#define EMAIL_FILE_NAME_1 "tcs-testfile-1.email"
#define EMAIL_FILE_NAME_0 "tcs-testfile-0.email"

#define PHONE_MALWARE_NAME MALWARE_1_3_0
#define PHONE_VARIANT_NAME VARIANT_1_3_0
#define PHONE_SEVERITY_CLASS TCS_SC_TERMINAL
#define PHONE_BEHAVIOR_CLASS TCS_BC_LEVEL3
#define PHONE_MALWARE_TYPE TCS_VTYPE_MALWARE
#define PHONE_FILE_NAME_1 "tcs-testfile-1.phone"
#define PHONE_FILE_NAME_0 "tcs-testfile-0.phone"

#define TEXT_MALWARE_NAME MALWARE_1_4_0
#define TEXT_VARIANT_NAME VARIANT_1_4_0
#define TEXT_SEVERITY_CLASS TCS_SC_TERMINAL
#define TEXT_BEHAVIOR_CLASS TCS_BC_LEVEL4
#define TEXT_MALWARE_TYPE TCS_VTYPE_MALWARE
#define TEXT_FILE_NAME_1 "tcs-testfile-1.txt"
#define TEXT_FILE_NAME_0 "tcs-testfile-0.txt"
#define MULTIPLE_BYTES_FILE_NAME_1 "tcs-testfile-1.mbytes.测试っせふぇㅎ손ㄷㅈ"
#define MULTIPLE_BYTES_FILE_NAME_0 "tcs-testfile-0.mbytes.测试っせふぇㅎ손ㄷㅈ"

#define JAVA_MALWARE_NAME MALWARE_1_7_0
#define JAVA_VARIANT_NAME VARIANT_1_7_0
#define JAVA_SEVERITY_CLASS TCS_SC_USER
#define JAVA_BEHAVIOR_CLASS TCS_BC_LEVEL0
#define JAVA_MALWARE_TYPE TCS_VTYPE_MALWARE
#define JAVA_FILE_NAME_1 "tcs-testfile-1.class"
#define JAVA_FILE_NAME_0 "tcs-testfile-0.class"

#define MULTIPLE0_MALWARE_NAME MALWARE_1_5_0
#define MULTIPLE0_VARIANT_NAME VARIANT_1_5_0
#define MULTIPLE0_SEVERITY_CLASS TCS_SC_USER
#define MULTIPLE0_BEHAVIOR_CLASS TCS_BC_LEVEL0
#define MULTIPLE0_MALWARE_TYPE TCS_VTYPE_MALWARE
#define MULTIPLE0_FILE_NAME_1 "tcs-testfile-1.multiple"
#define MULTIPLE0_FILE_NAME_0 "tcs-testfile-0.multiple"

#define MULTIPLE1_MALWARE_NAME MALWARE_1_6_0
#define MULTIPLE1_VARIANT_NAME VARIANT_1_6_0
#define MULTIPLE1_SEVERITY_CLASS TCS_SC_USER
#define MULTIPLE1_BEHAVIOR_CLASS TCS_BC_LEVEL1
#define MULTIPLE1_MALWARE_TYPE TCS_VTYPE_MALWARE
#define MULTIPLE1_FILE_NAME_1 "tcs-testfile-1.multiple"
#define MULTIPLE1_FILE_NAME_0 "tcs-testfile-0.multiple"

#define JAVAS_MALWARE_NAME MALWARE_1_8_0
#define JAVAS_VARIANT_NAME VARIANT_1_8_0
#define JAVAS_SEVERITY_CLASS TCS_SC_USER
#define JAVAS_BEHAVIOR_CLASS TCS_BC_LEVEL2
#define JAVAS_MALWARE_TYPE TCS_VTYPE_MALWARE
#define JAVAS_FILE_NAME_1 "tcs-testfile-1.js"
#define JAVAS_FILE_NAME_0 "tcs-testfile-0.js"

#define COMPRESS_MALWARE_NAME MALWARE_1_9_0
#define COMPRESS_VARIANT_NAME VARIANT_1_9_0
#define COMPRESS_SEVERITY_CLASS TCS_SC_USER
#define COMPRESS_BEHAVIOR_CLASS TCS_BC_LEVEL2
#define COMPRESS_MALWARE_TYPE TCS_VTYPE_MALWARE
#define COMPRESS_FILE_NAME_1 "tcs-testfile-1.z"
#define COMPRESS_FILE_NAME_0 "tcs-testfile-0.z"

#ifdef __cplusplus 
}
#endif

#endif /* SAMPLE_INFO_H */

