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

/**
 * \file TWPSerDaemon.c
 * \brief TWP Service Daemon source file.
 *
 * This file implements the TWP Service Daemon functions used by Security framework.
 */

#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "Debug.h"
#include "IpcForkDaemon.h"
#include "TWPImpl.h"
#include "TWPSerDaemon.h"

#define RISK_LEVEL_LEN 2
#define RETURN_STATUS_STR_LEN 4

int
TWPSerGetVersion(void *pData, int req_argc, char **req_argv, char ***rep_argv, int *rep_argc,
                   CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    // Opens TWP Framework library.
    TWPAPIInit Init;
    TWPLIB_HANDLE hLib;

    Init.api_version = TWPAPI_VERSION;
    Init.memallocfunc = (TWPFnMemAlloc) malloc;
    Init.memfreefunc = free;
    hLib = TWPInitLibrary(&Init);

    *rep_argc = 1;
    *rep_argv = NULL;

    TWP_RESULT iRes;
    char *szRes = NULL;

    if (hLib == INVALID_TWPLIB_HANDLE)
    {
        iRes = TWP_INVALID_HANDLE;
        goto close_library;
    }

    // Get the Version Information of TWP Framework.
    TWPVerInfo VerInfo;
    iRes = TWPGetVersion(hLib, &VerInfo);

    // Check if TWPGetVersion call returned TWP_SUCCESS.
    if (iRes != TWP_SUCCESS)
    {
        DERR("%s", "TWPGetVersion did not return TWP_SUCCESS\n");
    }
    else
    {
        DINFO("TWPGetVersion returns = %s %s\n", VerInfo.szFrameworkVer, VerInfo.szPluginVer);

        char *szRepArgv1 = strdup(VerInfo.szFrameworkVer);
        char *szRepArgv2 = strdup(VerInfo.szPluginVer);
        char *szRepArgv3 = strdup(TWP_DAEMON_VERSION);

        if (szRepArgv1 == NULL || szRepArgv2 == NULL || szRepArgv3 == NULL)
        {
            DERR("%s", "strdup fails to allocate mem for version info");

            free(szRepArgv1);
            free(szRepArgv2);
            free(szRepArgv3);
            iRes = TWP_NOMEM;
            goto close_library;
        }

        *rep_argc = 4;
        *rep_argv = (char **) malloc(sizeof(char*) * (*rep_argc));

        if (*rep_argv == NULL)
        {
            DERR("%s", "malloc returns Error for reply pointer array\n");

            free(szRepArgv1);
            free(szRepArgv2);
            free(szRepArgv3);
            *rep_argc = 1;
            iRes = TWP_NOMEM;
            goto close_library;
        }

        (*rep_argv)[1] = szRepArgv1;
        (*rep_argv)[2] = szRepArgv2;
        (*rep_argv)[3] = szRepArgv3;
    }

close_library:
    // Compose the return value.
    // Convert iRes to String.
    szRes = (char *) malloc(sizeof(char) * RETURN_STATUS_STR_LEN);
    if (szRes == NULL)
    {
        DERR("%s", "malloc returns error\n");
        goto err;
    }
    snprintf(szRes, RETURN_STATUS_STR_LEN, "%d", iRes);

    if (*rep_argc == 1)
    {
        *rep_argv = (char **) malloc (sizeof(char*) * 1);
        if (*rep_argv == NULL)
            goto err;
    }

    //Assign result code.
    (*rep_argv)[0] = szRes;

    TWPUninitLibrary(hLib);
    return 0;

err:
    if (*rep_argv != NULL)
    {
        free((*rep_argv)[1]);
        free((*rep_argv)[2]);
        free((*rep_argv)[3]);
    }
    *rep_argc = 0;
    free(*rep_argv);
    *rep_argv = NULL;
    rep_argv = NULL;
    free(szRes);
    TWPUninitLibrary(hLib);

    return -1;
}

int
TWPSerGetURLReputation(void *pData, int req_argc, char **req_argv, char ***rep_argv, int *rep_argc,
                   CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    // open TWP framework library
    TWPAPIInit Init;
    TWPLIB_HANDLE hLib;

    Init.api_version = TWPAPI_VERSION;
    Init.memallocfunc = (TWPFnMemAlloc) malloc;
    Init.memfreefunc = free;
    hLib = TWPInitLibrary(&Init);

    *rep_argc = 1;
    *rep_argv = NULL;

    TWP_RESULT iRes;
    char *szRes = NULL;

    if (hLib == INVALID_TWPLIB_HANDLE)
    {
        iRes = TWP_INVALID_HANDLE;
        goto close_library;
    }

    // Get Risk Level.
    int iRiskLevel;
    unsigned int uBlkUrlLen;
    char *pBlkUrl = NULL;

    // Check for Risk Level of URL.
    iRes = TWPCheckURL(hLib, req_argv[0], &pBlkUrl, &uBlkUrlLen, &iRiskLevel);

    // Check if TWPCheckURL call returned TWP_SUCCESS.
    if (iRes != TWP_SUCCESS)
    {
        DERR("%s : %s\n", "TWPCheckURL returned Error", req_argv[0]);
    }
    else
    {
        DINFO("TWPCheckURL returns risk level = %d\n", iRiskLevel);

        // convert RiskLevel integer to string
        char *szRiskLevel;
        szRiskLevel = (char *) malloc(sizeof(char) * RISK_LEVEL_LEN);
        if (szRiskLevel == NULL)
        {
            DINFO("%s", "malloc returns error\n");
            iRes = TWP_NOMEM;
            goto close_library;
        }
        snprintf(szRiskLevel, RISK_LEVEL_LEN, "%d", iRiskLevel);

        *rep_argc = 2;
        *rep_argv = (char **) malloc(sizeof(char*) * (*rep_argc));
        if (*rep_argv == NULL)
        {
            DINFO("%s", "realloc returns Error for reply pointer array\n");
            free(szRiskLevel);
            *rep_argc = 1;
            iRes = TWP_NOMEM;
            goto close_library;
        }

        //Assign Risk Level
        (*rep_argv)[1] = szRiskLevel;

        // Assign redirect url if RiskLevel is greater than or equal to TWP_Medium
        if (iRiskLevel >= TWP_Medium)
        {
            *rep_argc = 3;
            *rep_argv = (char **) realloc(*rep_argv, sizeof(char*) * (*rep_argc));
            if (*rep_argv == NULL)
            {
                DINFO("%s", "realloc returns Error for reply pointer array\n");
                free(szRiskLevel);
                *rep_argc = 1;
                iRes = TWP_NOMEM;
                goto close_library;
            }
            (*rep_argv)[2] = pBlkUrl;
        }
    }

close_library:
    // Compose the return value.
    // Convert iRes to String.
    szRes = (char *) malloc(sizeof(char) * RETURN_STATUS_STR_LEN);
    if (szRes == NULL)
    {
        DERR("%s", "malloc returns error\n");
        goto err;
    }
    snprintf(szRes, RETURN_STATUS_STR_LEN, "%d", iRes);

    if (*rep_argc == 1)
    {
        *rep_argv = (char **) malloc (sizeof(char*) * 1);
        if (*rep_argv == NULL)
            goto err;
    }
    //Assign Result code
    (*rep_argv)[0] = szRes;

    TWPUninitLibrary(hLib);
    return 0;

err:
    if (*rep_argv != NULL)
    {
        free((*rep_argv)[1]);
        free((*rep_argv)[2]);
    }
    *rep_argc = 0;
    free(*rep_argv);
    *rep_argv = NULL;	
    rep_argv = NULL;
    free(szRes);
    TWPUninitLibrary(hLib);
    return -1;
}

int
main(int argc, char **argv)
{
#ifndef DEBUG
    fork_daemon();
#endif

    TSC_SERVER_HANDLE hServer;

    if ((hServer = IpcServerOpen(TSC_DBUS_SERVER_WP_CHANNEL)) != NULL)
    {
        DINFO("%s", "successfully opened server \n");

        // Register methods for get url reputation
        IpcServerMethod method_1;
        snprintf(method_1.szMethod, sizeof(method_1.szMethod), "%s", "TWPSerGetURLReputation");
        method_1.method = (METHODFUNC) TWPSerGetURLReputation;
        method_1.pData = NULL;

        if (IpcServerAddMethod(hServer, &method_1) != 0)
        {
            DERR("%s", "unable to add method TWPSerGetURLReputation\n");
            goto close_server;
        }

        // Register methods for getversion
        IpcServerMethod method_2;
        snprintf(method_2.szMethod, sizeof(method_2.szMethod), "%s", "TWPSerGetVersion");
        method_2.method = (METHODFUNC) TWPSerGetVersion;
        method_2.pData = NULL;

        if (IpcServerAddMethod(hServer, &method_2) != 0)
        {
            DERR("%s", "unable to add method TWPSerGetVersion\n");
            goto close_server;
        }

        // Daemon waits here for request from clients.
        IpcServerMainLoop(hServer);

        IpcServerClose(&hServer);
    }
    else
    {
        DFATAL("%s", "unable to open server connection \n");
        goto err;
    }

    return 0;

close_server:
    IpcServerClose(&hServer);

err:
    DFATAL("%s", "Unable to start the Daemon \n");
    return -1;
}
