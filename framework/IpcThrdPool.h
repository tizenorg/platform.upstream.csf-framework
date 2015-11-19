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

#ifndef _IPCTHRDPOOL_H
#define _IPCTHRDPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "IpcServer.h"

/**
 *
 */
typedef struct IpcHandles_struct
{
    struct IpcHandles_struct *pNext;
    void *pMethodHandle;
    void *pData;
} IpcHandles;

/**
 *
 */
typedef struct IpcHandlePool_struct
{
    IpcHandles *pHList;
    pthread_mutex_t Lock;
    pthread_cond_t Cond;
    int iIdleCount;
} IpcHandlePool;

/**
 * Initializes thread pool. Returns 0 on success and -1 on failure.
 */
int IpcThrPoolInit(IpcHandlePool *pHPool, int iNumHandles);

/**
 * Frees the resources and nullifies the thread pool object.
 */
void IpcThrPoolFree(IpcHandlePool **pHPool);

/**
 * Synchronized method to returns a thread handle from the pool. The method is blocked till a
 * handle is available.
 */
IpcHandles *IpcThrPoolGet(IpcHandlePool *pHPool);

/**
 *
 */
void IpcThrPoolPut(IpcHandlePool *pHPool, IpcHandles *pHandle);

/**
 * Returns number of handles available in thread pool.
 * Returns -1 when pool is invalid.
 */
int IpcThrPoolIdleCount(IpcHandlePool *pHPool);


#ifdef __cplusplus
}
#endif

#endif

