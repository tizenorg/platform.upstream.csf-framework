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

#ifndef IPCSTRUCTS_H
#define IPCSTRUCTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file IpcStructs.h
 * \brief Header File for structures by Ipc.
 *
 * This file implements the structures needed by IPC Client, used by Security framework.
 */

#include <dbus/dbus.h>
#include <pthread.h>

#include "IpcMacros.h"
#include "IpcTypes.h"

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * Handle for message sent, to be used for future reference.
 */
typedef struct _ClientCallHandle
{
    char idUnique[MSGHANDLE_LEN];
    DBusPendingCall *pPendingCall;
    char service_name[TSC_SERVER_NAME_LEN + 1];
} ClientCallHandle;

/**
 * IPC client connection info.
 */
typedef struct _IpcClientInfo
{
    DBusConnection *dbconn;
    char req_name[TSC_REQ_STR_LEN];
    char pid[TSC_REQ_STR_LEN];
} IpcClientInfo;

/**
 * Data shared between the synchronized thread. It wraps the asynchronous call handle.
 */
typedef struct _SharedData
{
    int iSent;
    ClientCallHandle *pCallHandle;
    char szCallHandlePrefix[MSGHANDLE_LEN];
    DBusMessage *pMsg;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} SharedData;

/**
 * Data passed to threads sending messages asynchronously.
 * Owns only SharedData, rest are owned by 'parent thread'.
 */
typedef struct _ThreadData
{
    DBusConnection *pConn;
    int timeout_milliseconds;
    SharedData *pSharedData;
    TSCCallback pCallBack;
    void *pPrivate;
} ThreadData;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * _ClientCallHandle related.
 */
int _AssignToClientCallHandle(ClientCallHandle *pHandle, const char *szPrefix, dbus_uint32_t serial,
                              DBusPendingCall *pPendingCall);

int _CreateClientCallHandle(ClientCallHandle **ppHandle, const char *szPrefix, dbus_uint32_t serial,
                            DBusPendingCall *pPendingCall);

void _FreeClientCallHandle(ClientCallHandle *pHandle);

/**
 * SharedData related.
 */
void _FreeSharedData(SharedData *pSharedData);

// Create the handle to monitor send completion and receive sent message details.
// Maintains only reference of DBusMessage; Ownership is not changed.
SharedData *_CreateSharedData(const char *szCallHandlePrefix, DBusMessage *pMsg);

/**
 * ThreadData related.
 */
ThreadData *_AllocThreadData(DBusConnection *pConn, int timeout_milliseconds,
                             SharedData *pSharedData, TSCCallback pCallback, void *pPrivate);

void _FreeThreadData(void *pThreadData);

#ifdef __cplusplus
}
#endif

#endif  /* IPCSTRUCTS_H */

