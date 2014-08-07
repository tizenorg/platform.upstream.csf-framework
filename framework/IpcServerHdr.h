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

#ifndef IPCSERVERHDR_H
#define IPCSERVERHDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "IpcServer.h"
#include "IpcThrdPool.h"

#define SAFE_FREE(x)    if (x) {free(x); x = NULL;}
#define FREE(x)         if (x) {free(x);}

/**
 * Forward Declarations.
 */
/*struct IpcServerInfo;*/


/**
 * Keep registered methods list
 */
typedef struct _IpcServerMethodList
{
    IpcServerMethod *pMethod;
    struct _IpcServerMethodList *pNext;
} IpcServerMethodList;

/**
 * Keep the context for callback passed to stub method
 */
typedef struct _IpcMethodHandle
{
    IpcServerMethod *pMethod;
    void *pData;
    struct IpcServerInfo *pInfo;
    char unique_id[MSGHANDLE_LEN]; //uniquely identify the running method
    int iCancel;  // 1 is cancel, 0 not cancel
    char *cStatus;    // any status
    pthread_mutex_t Lock;

    struct _IpcMethodHandle *pNext;
} IpcMethodHandle;

/**
 *  Single context per server connection.
 */
typedef struct _IpcServerInfo
{
    char name[TSC_SERVER_NAME_LEN];
    DBusConnection *pConn;
    IpcServerMethodList *pMethodList;   // available methods list
    char rule[TSC_INFO_RULE_LEN];
    int fd;
    bool start_server_flag;
    DBusObjectPathVTable *pTable; //TODO: it is core dump if new thread the running method through pTable registered list
    pthread_t lDbus_listen_thread; //listen thread on the socket for reading message
    IpcHandlePool *pHandlePool; // thread pool for the running methods
    int count;
    IpcMethodHandle *pRunningMethods;  // current running methods
    pthread_mutex_t Lock;   // mutex to manage methods - iteration, add, remove.
} IpcServerInfo;


/**
 * Copy the data from the message read from the listening/reading socket thread, and pass it to the reply thread, where executing the method
 */
typedef struct _IpcAsyncInfo
{
    DBusConnection *pConn;
    IpcServerInfo *pInfo;
    DBusMessage *pMsg;
    int argc;
    char **argv;
    IpcServerMethod *pMethod;
    IpcHandles *pHandle;
    char async_unique_id[MSGHANDLE_LEN]; //uniquely identify the running method
} IpcAsyncInfo;

int _IpcServerInit(IpcServerInfo *pServerInfo, char *szServiceName);
void _IpcServerDeInit(TSC_SERVER_HANDLE hServer);
DBusHandlerResult _IpcServerReplyMessage(DBusConnection *pConn, DBusMessage *pMsg,
                                         char **pReply, int size);
DBusHandlerResult _IpcServerMsgFilter(DBusConnection *pConn, DBusMessage *pMsg, void *pData);
int _ParseDBusMessage(DBusMessage *pMsg, int *argc, char ***argv);
DBusHandlerResult _IpcServerMsgHandler(void *data);
DBusHandlerResult _IpcServerProcessMessage(void *data);
void *_IpcPopMessage(void *hServer);
void CleanupArray(char*** pArr, int len);
void CleanupAsync(IpcAsyncInfo *pAsync);
void FreeIpcServerHandle(TSC_SERVER_HANDLE hServer);
DBusHandlerResult _IpcServerReplyError(DBusConnection *pConn, DBusMessage *pMsg,
                                       int iErrCode);
int IpcCancelMethod(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle);
int IpcGetProgressMethod(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle);
int IpcShutdown(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle);
int IpcServerCallbackMethod(TSC_METHOD_HANDLE *pMHandle, TSC_METHOD_REASON_CODE iReason, void *reason_params);
int _RunDetachedThread(void *pfWorkerFunc, void *pThreadData);


void _FreeHandleMethods(IpcServerInfo *pServerInfo);
void _FreeHandlePool(IpcServerInfo *pServerInfo);
void _FreeHandleTable(IpcServerInfo *pServerInfo);
void _FreeHandleConn(IpcServerInfo *pServerInfo);
void _WaitForListenThreadClose(IpcServerInfo *pServerInfo);

#ifdef __cplusplus
}
#endif

#endif

