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

#ifndef TWPSERDAEMON_H
#define TWPSERDAEMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file TWPSerDaemon.h
 * \brief TWP Server Daemon Header file.
 *
 * This file has the function prototypes.
 * These functions should not be called directly but instead through IpcClient.h APIs
 */

#include "IpcServer.h"

#define TWP_DAEMON_VERSION "1.0.0"

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
/**
 * \brief handler method to get the version information.
 * This will be called by IpcServer library with the following input/output params.
 * This can be both synchronous and asynchronous API.
 *
 * \param[in] pData - contextual data.
 * \param[in] req_argc - set to 0.
 * \param[in] req_argv - set to null.
 * \param[out] rep_argc - set to 0, 1 or 4.
 * \param[out] rep_argv - contains  0, 1 or 4 strings for result code, framework version, plugin version, daemon version.
 * \param[in] callback - method callback.
 * \param[in] handle - handler.
 * \return Return Type (int) \n
 */
int TWPSerGetVersion(void *pData, int req_argc, char **req_argv, char ***rep_argv, int *rep_argc,
                        CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle);

/**
 * \brief handler method to get the URL Reputation.
 * This will be called by IpcServer library with the following input/output params.
 * This can be both synchronous and asynchronous API.
 *
 * \param[in] pData - contextual data.
 * \param[in] req_argc - set to 1.
 * \param[in] req_argv - set to the URL required to check reputation.
 *
 * \param[out] rep_argc - set to 0, 1, 2 or 3.
 * \param[out] rep_argv - contains 0, 1, 2  or 3 strings, first is the result code, second is the risklevel, third is the redirect URL.
 *                        redirect URL is returned only if risk level is greater than or equal to TWP_Medium.
 * \param[in] callback - method callback.
 * \param[in] handle - handler.
 * \return Return Type (int) \n
 */
int TWPSerGetURLReputation(void *pData, int req_argc, char **req_argv, char ***rep_argv, int *rep_argc,
                               CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle);


#ifdef __cplusplus
}
#endif

#endif  /* TWPSERDAEMON_H */
