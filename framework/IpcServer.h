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

#ifndef IPCSERVER_H
#define IPCSERVER_H

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * \file IpcServer.h
 * \brief Ipc Server Header File
 *  
 * This file provides the IPC Server API functions used by Security framework.
 */

#include <dbus/dbus.h>
#include <stdbool.h>
#include "IpcMacros.h"


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * Pointer to method requested by client module.
 */
/*typedef int (*CALLBACKFUNC) (void *pHandle, TSC_METHOD_REASON_CODE iReason, void *user_data);*/
typedef int (*CALLBACKFUNC) (void *pHandle, int iReason, void *reason_params);

typedef int (*METHODFUNC) (void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, void *pHandle);

/**
 * Server side method to handle request from client module. All the requests 
 * are managed in a list.
 */
typedef struct _IpcServerMethod
{
    char       szMethod[TSC_METHOD_NAME_LEN];
    METHODFUNC method;
    void       *pData; //when execute the method, the pData besides the data passed from client, also has methodHandle for this method
} IpcServerMethod;

#define TSC_NULL ((void *) 0)

#define TSCSERVERHANDLE(n) struct n##_struct {int iDummy;}; typedef struct n##_struct *n
TSCSERVERHANDLE(TSC_SERVER_HANDLE);

#define TSCMETHODHANDLE(n) struct n##_struct {int iDummy;}; typedef struct n##_struct *n
TSCMETHODHANDLE(TSC_METHOD_HANDLE);

#define INVALID_TSC_METHOD_HANDLE ((TSC_METHOD_HANDLE) TSC_NULL)

#define INVALID_TSC_SERVER_HANDLE ((TSC_SERVER_HANDLE) TSC_NULL)

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * \brief Adds a handler method to the list of methods at server, to process 
 * request from the client.
 *
 * During initialisation, the server builds a list of handlers to process
 * request coming from client-side IPC. Later the client side IPC sends request
 * along with the name of the handler to use. The handlers are implemented
 * in the module hosting the server-side IPC.
 *
 * This is a synchronous API.
 *
 * \param[in] pMethod Details of the handler method to be added at server-side.
 *
 * \return Return Type (void) \n
 *
 */
int IpcServerAddMethod(TSC_SERVER_HANDLE hServer, IpcServerMethod *pMethod);

/**
 * \brief Removes a handler method from the list of methods at server.
 *
 * During initialisation, the server builds a list of handlers to process
 * request coming from client-side IPC. Later the client side IPC sends request
 * along with the name of the handler to use. The handlers are implemented
 * in the module hosting the server-side IPC. As part of un-initialisation
 * the handler should be removed from the list.
 *
 * This is a synchronous API.
 *
 * \param[in] pMethod Pointer to handler method to be removed.
 *
 * \return Return Type (void) \n
 *
 */
int IpcServerRemoveMethod(TSC_SERVER_HANDLE hServer, METHODFUNC pMethod);


/*
void IpcCancelMethod(TSC_SERVER_HANDLE hServer, char *method_unique_id);
char *IpcGetProgressMethod(TSC_SERVER_HANDLE hServer, char *method_unique_id);
*/
/**
 * \brief Initialises the server-side IPC to make it ready for request from
 * client side IPC.
 *
 * The IPC has two parts - client and server. The server-side IPC processes the
 * request sent from the client-side IPC. The server-side uses handlers provided
 * by the hosting module. When server comes up, the IPC needs to be initialised
 * and be ready for the requests.
 *
 * This is a synchronous API.
 *
 * \return Return Type (int) \n
 * 0 - on send success. \n
 * -1 - on send failure. \n
 */
TSC_SERVER_HANDLE IpcServerOpen(char *servie_name);

/**
 * \brief Close the server-side IPC and release the resources.
 *
 * This is a synchronous API.
 *
 * \return Return Type (void) \n
 */
void IpcServerClose(TSC_SERVER_HANDLE*);

int IpcServerMainLoop(TSC_SERVER_HANDLE hServer);

/**
 * Callback function for Server Stub in cancel the method, update method progress
 */



#ifdef __cplusplus
}
#endif 

#endif  /* IPCSERVER_H */

