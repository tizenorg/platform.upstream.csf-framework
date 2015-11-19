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
 * \file IpcThrdPool.c
 * \brief Ipc Thread pool Ipc Source File
 *
 * This file implements the thread pool used by IPC Server in Security framework.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "Debug.h"
#include "IpcThrdPool.h"

static void IpcThrPoolReset(IpcHandlePool *pHPool)
{
    IpcHandles *pHandle;

    if (!pHPool)
        return;

    while ((pHandle = pHPool->pHList) != NULL)
    {
        pHPool->pHList = pHPool->pHList->pNext;
        free(pHandle);
    }

    pthread_cond_destroy(&pHPool->Cond);
    pthread_mutex_destroy(&pHPool->Lock);
    pHPool->iIdleCount = 0;
}

/**
 * Frees the resources and nullifies the thread pool object.
 */
void IpcThrPoolFree(IpcHandlePool **pHPool)
{
    IpcThrPoolReset(*pHPool);
    free(*pHPool);
    *pHPool = NULL;
}

/**
 * Creates a new handle and makes it head of the pool list.
 * Return 0 on success and -1 on failure.
 */
static int AddHandleToThrPool(IpcHandlePool *pHPool)
{
    IpcHandles *pHandle = NULL;
    if ((pHandle = (IpcHandles *) calloc(1, sizeof(IpcHandles))) == NULL)
    {
        DDBG("%s\n", "calloc IpcHandles");
        return -1;
    }


    pHandle->pNext = pHPool->pHList;
    pHPool->pHList = pHandle;
    return 0;
}

/**
 * Initializes thread pool. Returns 0 on success and -1 on failure.
 */
int IpcThrPoolInit(IpcHandlePool *pHPool, int iNumHandles)
{
    int i;
    int result = 0;

    if (!pHPool)
        return -1;

    if (pthread_mutex_init(&pHPool->Lock, NULL))
    {
        DDBG("%s\n", "mutex init");
        return -1;
    }


    if (pthread_cond_init(&pHPool->Cond, NULL))
    {
        pthread_mutex_destroy(&pHPool->Lock);
        DDBG("%s\n", "cond_init");
        return -1;
    }

    pHPool->iIdleCount = iNumHandles;
    pHPool->pHList = NULL;

    for (i = 0; i < iNumHandles; i++)
    {
        if (0 != AddHandleToThrPool(pHPool))
        {
            DDBG("%s\n", "add to thrpool");
            result = -1;
            break;
        }
    }

    return result;
}

/**
 * Synchronized method to returns a thread handle from the pool. The method is blocked till a
 * handle is available.
 */
IpcHandles *IpcThrPoolGet(IpcHandlePool *pHPool)
{
    IpcHandles *pHandle;
    pthread_mutex_lock(&pHPool->Lock);
    while ((pHandle = pHPool->pHList) == NULL)
    {
        pthread_cond_wait(&pHPool->Cond, &pHPool->Lock);
    }
    pHPool->pHList = pHandle->pNext;
    (pHPool->iIdleCount)--;
    pthread_mutex_unlock(&pHPool->Lock);

    return pHandle;
}

void IpcThrPoolPut(IpcHandlePool *pHPool, IpcHandles *pHandle)
{
    pthread_mutex_lock(&pHPool->Lock);
    pHandle->pNext = pHPool->pHList;
    pHPool->pHList = pHandle;
    (pHPool->iIdleCount)++;

    pthread_cond_broadcast(&pHPool->Cond);
    pthread_mutex_unlock(&pHPool->Lock);
}

/**
 * Returns number of handles available in thread pool.
 * Returns -1 when pool is invalid.
 */
int IpcThrPoolIdleCount(IpcHandlePool *pHPool)
{

    return pHPool? pHPool->iIdleCount : -1;
}

