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

#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * \file IpcClient.h
 * \brief Ipc Client Header File
 *  
 * This file provides the IPC Client API functions used by Security framework.
 */

#include <dbus/dbus.h>

#include "IpcMacros.h"


/*==================================================================================================
                                 CONSTANTS & ENUMS
==================================================================================================*/
#define DEF_TIMEOUT -1

/*==================================================================================================
                                 FORWARD DECLARATIONS
==================================================================================================*/
struct _ClientCallHandle;

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * Asynchronous call handle.
 */
typedef struct _ClientCallHandle * TSC_CALL_HANDLE;

/**
 * Data structure to encapsulate client side information of the IPC.
 */
typedef struct _IpcClientInfo IpcClientInfo;

/**
 * CallBack Function type for Async method supported by the IPC.
 *
 * \param[in] pPrivate API caller's context information, supplied with TSCSendMessageAsync earlier.
 * \param[in] argc Length of the string in argv.
 * \param[in] argv Array of strings representing result value of asynchronous reply.
 */
typedef void (*TSCCallback)(void *pPrivate, int argc, char **argv);

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * \brief Initializes and returns IPC info of client.
 *
 * Opens and initialises the client side IPC structure using the security 
 * framework defaults.
 *
 * This is a synchronous API.
 *
 * \return Return Type (IpcClientInfo*) \n
 * Pointer to structure containing IPC info - on success. \n
 * NULL - on failure. \n
 */
IpcClientInfo* IpcClientOpen(void);

/**
 * \brief Requests the Security framework's IPC server and returns back the reply.
 *
 * This is a synchronous API.
 *
 * \param[in] pInfo Client side IPC info returned by IpcClientOpen().
 *
 * \return Return Type (void) \n
 */
void IpcClientClose(IpcClientInfo *pInfo);

/**
 * \brief Requests the Security framework's IPC server and returns back the reply.
 *
 * This is a synchronous API.
 *
 * \param[in] pInfo Client side IPC info.
 * \param[in] szMethod Name of the method called.
 * \param[in] argc Number of parameters passed in argv.
 * \param[in] argv Array of strings representing parameters for method called.
 * \param[out] reply_len Length of the string in reply_argv.
 * \param[out] reply_argv Array of strings representing result value from method called.
 * \param[in] timeout_milliseconds Timeout in milliseconds. -1 for default or 0 for no timeout.
 *
 * \return Return Type (int) \n
 * 0 - on send success. \n
 * -1 - on send failure. \n
 */
int TSCSendMessageN(IpcClientInfo *pInfo, const char *service_name, const char *szMethod, int argc,
                    char **argv, int *argc_reply, char ***argv_reply, int timeout_milliseconds);

/**
 * \brief Requests the Security framework's IPC server asynchronously.
 *
 * This is an asynchronous API.
 *
 * \param[in] pInfo Client side IPC info.
 * \param[in] szMethod Name of the method called.
 * \param[in] argc Number of parameters passed in argv.
 * \param[in] argv Array of strings representing parameters for method called.
 * \param[out] pCallHandle Pointer to handle of the asynchronous message sent.
 * \param[in] pCallback Callback function for the asynchronous reply.
 * \param[in] pPrivate API caller's context information, to be supplied with callback.
 * \param[in] timeout_milliseconds Timeout in milliseconds. -1 for default or 0 for no timeout.
 *
 * \return Return Type (int) \n
 * 0 - on send success. \n
 * Error code - on send failure. \n
 */
int TSCSendMessageAsync(IpcClientInfo *pInfo, const char *service_name, const char *szMethod, int argc, char **argv,
                        TSC_CALL_HANDLE *pCallHandle, TSCCallback pCallback, void *pPrivate,
                        int timeout_milliseconds);

/**
 * \brief Releases the asynchronous call handle.
 *
 * \param[in] callHandle handle of the asynchronous message sent earlier.
 */
void TSCFreeSentMessageHandle(TSC_CALL_HANDLE callHandle);

/**
 * Cancels an asynchronous request previously made to the Security framework's IPC server.
 * On success, releases the handle of the previously called asynchronous method.
 *
 * This is an asynchronous API.
 *
 * \param[in] pInfo Client side IPC info.
 * \param[in] callHandle handle of the asynchronous message sent earlier.
 *
 * \return Return Type (int) \n
 * 0 - on send success. \n
 * Error code - on failure. \n
 */
int TSCCancelMessage(IpcClientInfo *pInfo, TSC_CALL_HANDLE callHandle);

#ifdef __cplusplus
}
#endif 

#endif  /* IPCCLIENT_H */
