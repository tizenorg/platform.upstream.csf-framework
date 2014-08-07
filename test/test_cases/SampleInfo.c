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

#include "stdlib.h"
#include "TCSTest.h"
#include "TCSImpl.h"

#include "SampleInfo.h"


/*
 * Maximum sample file name length.
 */
#define MAX_SAMPLE_NAME_LEN 128

#define MAX_INFECTED_NUM 3


typedef struct SampleInfo_struct
{
    enum ENUM_MALWARE_TEST_TYPES eTestType;
    char szName[TCS_MAX_MALWARE_NAME_LEN];
    char szVariant[TCS_MAX_MALWARE_NAME_LEN];
    unsigned int uSeverity;
    unsigned int uBehavior;
    unsigned int uType;
    char szInfectedFileName[MAX_SAMPLE_NAME_LEN];
    char szBenignFileName[MAX_SAMPLE_NAME_LEN];
} SampleInfo;


static SampleInfo Samples[][MAX_INFECTED_NUM] =
{
    {
        {
            MALWARE_TTYPE_BUFFER,
            BUFFER_MALWARE_NAME,
            BUFFER_VARIANT_NAME,
            BUFFER_SEVERITY_CLASS,
            BUFFER_BEHAVIOR_CLASS,
            BUFFER_MALWARE_TYPE,
            BUFFER_FILE_NAME_1,
            BUFFER_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_HTML,
            HTML_MALWARE_NAME,
            HTML_VARIANT_NAME,
            HTML_SEVERITY_CLASS,
            HTML_BEHAVIOR_CLASS,
            HTML_MALWARE_TYPE,
            HTML_FILE_NAME_1,
            HTML_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_URL,
            URL_MALWARE_NAME,
            URL_VARIANT_NAME,
            URL_SEVERITY_CLASS,
            URL_BEHAVIOR_CLASS,
            URL_MALWARE_TYPE,
            URL_FILE_NAME_1,
            URL_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_EMAIL,
            EMAIL_MALWARE_NAME,
            EMAIL_VARIANT_NAME,
            EMAIL_SEVERITY_CLASS,
            EMAIL_BEHAVIOR_CLASS,
            EMAIL_MALWARE_TYPE,
            EMAIL_FILE_NAME_1,
            EMAIL_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_PHONE,
            PHONE_MALWARE_NAME,
            PHONE_VARIANT_NAME,
            PHONE_SEVERITY_CLASS,
            PHONE_BEHAVIOR_CLASS,
            PHONE_MALWARE_TYPE,
            PHONE_FILE_NAME_1,
            PHONE_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_TEXT,
            TEXT_MALWARE_NAME,
            TEXT_VARIANT_NAME,
            TEXT_SEVERITY_CLASS,
            TEXT_BEHAVIOR_CLASS,
            TEXT_MALWARE_TYPE,
            TEXT_FILE_NAME_1,
            TEXT_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_JAVA,
            JAVA_MALWARE_NAME,
            JAVA_VARIANT_NAME,
            JAVA_SEVERITY_CLASS,
            JAVA_BEHAVIOR_CLASS,
            JAVA_MALWARE_TYPE,
            JAVA_FILE_NAME_1,
            JAVA_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_MULTIPLE,
            MULTIPLE0_MALWARE_NAME,
            MULTIPLE0_VARIANT_NAME,
            MULTIPLE0_SEVERITY_CLASS,
            MULTIPLE0_BEHAVIOR_CLASS,
            MULTIPLE0_MALWARE_TYPE,
            MULTIPLE0_FILE_NAME_1,
            MULTIPLE0_FILE_NAME_0
        },
        {
            MALWARE_TTYPE_MULTIPLE,
            MULTIPLE1_MALWARE_NAME,
            MULTIPLE1_VARIANT_NAME,
            MULTIPLE1_SEVERITY_CLASS,
            MULTIPLE1_BEHAVIOR_CLASS,
            MULTIPLE1_MALWARE_TYPE,
            MULTIPLE1_FILE_NAME_1,
            MULTIPLE1_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_JAVAS,
            JAVAS_MALWARE_NAME,
            JAVAS_VARIANT_NAME,
            JAVAS_SEVERITY_CLASS,
            JAVAS_BEHAVIOR_CLASS,
            JAVAS_MALWARE_TYPE,
            JAVAS_FILE_NAME_1,
            JAVAS_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_COMPRESS,
            COMPRESS_MALWARE_NAME,
            COMPRESS_VARIANT_NAME,
            COMPRESS_SEVERITY_CLASS,
            COMPRESS_BEHAVIOR_CLASS,
            COMPRESS_MALWARE_TYPE,
            COMPRESS_FILE_NAME_1,
            COMPRESS_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
    {
        {
            MALWARE_TTYPE_MULTIPLE_BYTES,
            TEXT_MALWARE_NAME,
            TEXT_VARIANT_NAME,
            TEXT_SEVERITY_CLASS,
            TEXT_BEHAVIOR_CLASS,
            TEXT_MALWARE_TYPE,
            MULTIPLE_BYTES_FILE_NAME_1,
            MULTIPLE_BYTES_FILE_NAME_0
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        },
        {
            -1, "", "", 0, 0, 0, "", ""
        }
    },
};


static SampleInfo *SampleGetInfo(int iType, int iIndex)
{
    int i;
    int n = (int) (sizeof(Samples) / (sizeof(SampleInfo) * MAX_INFECTED_NUM));

    for (i = 0; i < n; i++)
    {
        if (Samples[i][0].eTestType == iType)
            return &Samples[i][iIndex];
    }

    return NULL;
}


const char *SampleGetMalName(int iType, int iIndex)
{

    return SampleGetInfo(iType, iIndex)->szName;
}


const char *SampleGetVarName(int iType, int iIndex)
{

    return SampleGetInfo(iType, iIndex)->szVariant;
}


unsigned int SampleGetSeverity(int iType, int iIndex)
{

    return SampleGetInfo(iType, iIndex)->uSeverity;
}


unsigned int SampleGetMalType(int iType, int iIndex)
{

    return SampleGetInfo(iType, iIndex)->uType;
}


unsigned int SampleGetBehavior(int iType, int iIndex)
{

    return SampleGetInfo(iType, iIndex)->uBehavior;
}


const char *SampleGetInfectedFileName(int iType)
{

    return SampleGetInfo(iType, 0)->szInfectedFileName;
}


const char *SampleGetBenignFileName(int iType)
{

    return SampleGetInfo(iType, 0)->szBenignFileName;
}


int SampleGetCount(int iType)
{
    int i = 0;

    while (SampleGetInfo(iType, i) && SampleGetInfo(iType, i)->eTestType == iType)
        i++;

    return i;
}

