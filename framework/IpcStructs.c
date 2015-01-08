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
 * \file IpcStructs.c
 * \brief Source File for structures needed by Ipc.
 *  
 * This file implements the structures needed by IPC Client, used by Security framework.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "IpcStructs.h"

/**
 * Assign new fields to existing call handle.
 * A NULL pPendingCall will not replace the existing one.
 */
int _AssignToClientCallHandle(ClientCallHandle *pHandle, const char *szPrefix, dbus_uint32_t serial,
                              DBusPendingCall *pPendingCall)
{
    int iRet = 1;

    do
    {
        if (!pHandle)
            break;

        if (0 > (iRet = snprintf(pHandle->idUnique, MSGHANDLE_LEN, TSC_MID_SVR_FORMAT, szPrefix,
                                 serial)))
            break;

        if (pPendingCall)
            pHandle->pPendingCall = pPendingCall;

        iRet = 0;

    } while(0);

    return iRet;
}

int _CreateClientCallHandle(ClientCallHandle **ppHandle, const char *szPrefix, dbus_uint32_t serial,
                            DBusPendingCall *pPendingCall)
{
    int iRet = 1;
    ClientCallHandle *pHandle = NULL;

    do
    {
        if (!ppHandle)
            break;

        pHandle = (ClientCallHandle *) malloc(sizeof(ClientCallHandle));
        if (!pHandle)
            break;

        if ((iRet = _AssignToClientCallHandle(pHandle, szPrefix, serial, pPendingCall)))
            break;

        *ppHandle = pHandle;
        iRet = 0;

    } while(0);

    if (iRet && pHandle)
    {
        free(pHandle);
        pHandle = NULL;
    }

    return iRet;
}

void _FreeClientCallHandle(ClientCallHandle *pHandle)
{
    free(pHandle);
}

void _FreeSharedData(SharedData *pSharedData)
{
    if (pSharedData)
    {
        pthread_mutex_destroy(&pSharedData->lock);
        pthread_cond_destroy(&pSharedData->cond);
        free(pSharedData);
    }
}

// Create the handle to monitor send completion and receive sent message details.
// Maintains only reference of DBusMessage; Ownership is not changed.
SharedData *_CreateSharedData(const char *szCallHandlePrefix, DBusMessage *pMsg)
{
    int iDone = 0;
    SharedData *pSharedData = NULL;

    do
    {
        if (!pMsg)
            break;

        if ((pSharedData = (SharedData *)calloc(1, sizeof(SharedData))) == NULL)
            break;

        if (0 > snprintf(pSharedData->szCallHandlePrefix, MSGHANDLE_LEN, "%s", szCallHandlePrefix))
            break;

        pSharedData->pMsg = pMsg;
        pSharedData->pCallHandle = NULL;
        pSharedData->iSent = 0;

        if (pthread_mutex_init(&(pSharedData->lock), NULL))
            break;

        if (pthread_cond_init(&(pSharedData->cond), NULL))
            break;

        iDone = 1;
    } while(0);

    if (!iDone)
    {
        _FreeSharedData(pSharedData);
        return NULL;
    }
    return pSharedData;
}

ThreadData *_AllocThreadData(DBusConnection *pConn, int timeout_milliseconds,
                             SharedData *pSharedData, TSCCallback pCallback, void *pPrivate)
{
    ThreadData *pThreadData = (ThreadData *)dbus_malloc(sizeof(ThreadData));
    if (!pThreadData)
        return NULL;

    pThreadData->pConn = pConn;
    pThreadData->timeout_milliseconds = timeout_milliseconds;
    pThreadData->pSharedData = pSharedData;
    pThreadData->pCallBack = pCallback;
    pThreadData->pPrivate = pPrivate;

    return pThreadData;
}

void _FreeThreadData(void *pThreadData)
{
    if (pThreadData)
    {
        _FreeSharedData(((ThreadData*)pThreadData)->pSharedData);
    }
    dbus_free(pThreadData);
}


#include "IpcStructs.h"
