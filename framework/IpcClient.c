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
 * \file IpcClient.c
 * \brief Ipc Client Source File
 *  
 * This file implements the IPC Client API functions used by Security framework.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "IpcStructs.h"
#include "TSCErrorCodes.h"

static DBusHandlerResult _IpcClientMsgFilter(DBusConnection *dbconn, DBusMessage *dbmsg, void *data);
static bool _IpcClientInit(IpcClientInfo *pInfo);
static void _IpcClientDeInit(IpcClientInfo *pInfo);
static void _IpcHandleAsyncReply(DBusPendingCall *pPendingCall, void *pThreadData);

#ifdef DEBUG
#define DEBUG_LOG(_fmt_, _param_...)    \
    { \
        fprintf(stderr, "[TSC Client] %s %d " _fmt_, __FILE__, __LINE__, _param_); \
    }
#else
#define DEBUG_LOG(_fmt_, _param_...)
#endif


/**
 * Initializes and returns IPC info of client.
 */
IpcClientInfo* IpcClientOpen(void)
{
    IpcClientInfo *pInfo = NULL;
    pInfo = calloc(1, sizeof(IpcClientInfo));
    if (pInfo == NULL)
        return NULL;

    snprintf(pInfo->pid, TSC_REQ_STR_LEN, "%d", getpid());
    snprintf(pInfo->req_name, TSC_REQ_STR_LEN, "%s%d", TSC_DBUS_CLIENT, getpid());

    // Init Dbus Client
    if (!_IpcClientInit(pInfo))
        goto err_conn;

    return pInfo;

err_conn:
    if (pInfo)
        free(pInfo);

    return NULL;
}

/**
 * Close the client-side IPC and release the resources.
 */
void IpcClientClose(IpcClientInfo *pInfo)
{
    if (!pInfo)
        return;

    _IpcClientDeInit(pInfo);
    free(pInfo);
}

/**
 * Gives the prefix for the unique id of message sent.
 */
char *_GetMsgIdPrefix(void)
{
    char szIdPrefix[MSGHANDLE_LEN] = {0};
    if (snprintf(szIdPrefix, MSGHANDLE_LEN, TSC_MID_PREFIX_FORMAT, getpid(), pthread_self()) >= 0)
        return strdup(szIdPrefix);

    return NULL;
}

/**
 * Requests the Security framework's IPC server and returns back the reply.
 */
int TSCSendMessageN(IpcClientInfo *pInfo, const char *service_name, const char *szMethod, int argc,
                    char **argv, int *argc_reply, char ***argv_reply, int timeout_milliseconds)
{
    DBusMessage *dbmsg = NULL;
    DBusMessage *reply_msg = NULL;
    DBusMessageIter dbiter;
    DBusError dberr;
    char *pArgItem = NULL;
    char *pBuf = NULL;
    int i = 0;
    int j = 0;
    int iErr = TSC_ERROR_MODULE_GENERIC;
    int iSize = TSC_REPLY_MSG_COUNT_AVERAGE;

    *argc_reply = 0;

    // TODO: Avoid multiple exits.
    if (pInfo == NULL)
        return TSC_ERROR_INVALID_HANDLE;

    if (pInfo->dbconn == NULL)
        return TSC_ERROR_INVALID_HANDLE;

    if (argc < 0 || !szMethod)
        return TSC_ERROR_INVALID_PARAM;

    *argv_reply = malloc(iSize * sizeof(char *));
    if (!*argv_reply)
    {
        iErr = TSC_ERROR_INSUFFICIENT_RES;
        goto err_send;
    }

    dbus_error_init(&dberr);

    dbmsg = dbus_message_new_method_call(service_name, TSC_DBUS_PATH, TSC_DBUS_INTERFACE, szMethod);
    if (dbmsg == NULL)
    {
        iErr = TSC_ERROR_INSUFFICIENT_RES;
        goto err_send;
    }
    dbus_message_iter_init_append(dbmsg, &dbiter);

    // Add the message unique id as first element (the prefix).
    char *szIdPrefix = _GetMsgIdPrefix();
    if (!szIdPrefix || !dbus_message_iter_append_basic(&dbiter, DBUS_TYPE_STRING, &szIdPrefix))
    {
        DEBUG_LOG("client_lib: Failed to append id prefix for %s.\n", szMethod);
        free(szIdPrefix);
        goto err_send;
    }
    free(szIdPrefix);
    for (i = 0; i < argc; i++)
    {
        if (!dbus_validate_utf8(argv[i], &dberr))
        {
            DEBUG_LOG("%s", "client_lib: Not valid utf8 string\n");
            goto err_send;
        }
        if (!dbus_message_iter_append_basic(&dbiter, DBUS_TYPE_STRING, &argv[i]))
        {
            DEBUG_LOG("client_lib: %s failed to append arguments\n", pInfo->pid);
            goto err_send;
        }
    }

    if (timeout_milliseconds == 0)
        timeout_milliseconds = DBUS_TIMEOUT_INFINITE;

    reply_msg = dbus_connection_send_with_reply_and_block(pInfo->dbconn, dbmsg, 
                                                          timeout_milliseconds,
                                                          &dberr);

    if (dbus_error_is_set(&dberr))
    {
        DEBUG_LOG("client_lib: Failed to end %s\n", dberr.message);
        goto err_send;
    }

    if (reply_msg == NULL)
    {
        DEBUG_LOG("%s\n", "client_lib: reply message is NULL");
        goto err_send;
    }

    j = 0;
    if (!dbus_message_iter_init(reply_msg, &dbiter))
    {
        DEBUG_LOG("%s\n", "client_lib: Message has no arguments.");
        goto zero_args;
    }

    do
    {
        if (dbus_message_iter_get_arg_type(&dbiter) != DBUS_TYPE_STRING)
        {
            DEBUG_LOG("client_lib: %s argument is not string\n", pInfo->pid);
            goto err_send;
        }

        dbus_message_iter_get_basic(&dbiter, &pArgItem);
        if (!pArgItem)
        {
            DEBUG_LOG("client_lib: %s arg is NULL\n", pInfo->pid);
            goto err_send;
        }

        if (!(pBuf = strdup((const char*)pArgItem)))
        {
            goto err_send;
        }
        (*argv_reply)[j++] = pBuf;

        if (j >= iSize)
        {
            iSize += TSC_REPLY_MSG_COUNT_AVERAGE;
            *argv_reply = realloc(*argv_reply, sizeof(char*) * iSize);
            if (!*argv_reply)
            {
                goto err_send;
            }
        }
    } while (dbus_message_iter_has_next(&dbiter) && dbus_message_iter_next(&dbiter));

zero_args:
    *argc_reply = j;
    return 0;

err_send:
    if (*argv_reply)
    {
        while (j)
            free((*argv_reply)[--j]);

        free(*argv_reply);
        *argv_reply = NULL;
    }
    if (dbmsg)
        dbus_message_unref(dbmsg);
    if(reply_msg)
        dbus_message_unref(reply_msg);

    return -1;
}

/**
 * Block current thread till message is sent and structure is updated in child thread.
 */
void BlockTillSent(SharedData *pSharedData)
{
    pthread_mutex_lock(&pSharedData->lock);
    while (pSharedData->iSent == 0)
        pthread_cond_wait(&pSharedData->cond, &pSharedData->lock);
    pthread_mutex_unlock(&pSharedData->lock);
}

/**
 * Unblock parent thread as the message structures have been updated.
 * Unblocks only 1 thread associated with the condition variable.
 * Broadcast avoided as only waiting thread is expected.
 */
void UnblockOnSent(SharedData *pSharedData)
{
    pthread_mutex_lock(&pSharedData->lock);
    pSharedData->iSent = 1;
    pthread_cond_signal(&pSharedData->cond);
    pthread_mutex_unlock(&pSharedData->lock);
}

/**
 * Creates a thread for asynchronous task.
 * TODO: Merge with TCS module.
 */
int _RunDetachedThread(void *pfWorkerFunc, void *pThreadData)
{
    pthread_t thread;
    pthread_attr_t attr;

    int rc = -1;
    do
    {
        if (pthread_attr_init(&attr) != 0)
            break;

        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
            break;

        if (pthread_create(&thread, &attr, pfWorkerFunc, pThreadData) != 0)
            break;

        if (pthread_attr_destroy(&attr) != 0)
        {
            // As the thread is already running, return different error code.
            rc = -2;
            break;
        }

        rc = 0;
    }
    while (0);

    return rc;
}

void *_SendMessageWorker(void *pData)
{
    dbus_bool_t result;
    DBusPendingCall *pPendingCall = NULL;
    ThreadData *pThreadData = (ThreadData *) pData;
    ThreadData *pPendingData = NULL;
    int iSentFailed = 1;

    do
    {
        if (pThreadData == NULL)
        {
            DEBUG_LOG("%s\n", "Nothing to send from thread. Deadlock!!");
            break;
        }

        // Duplicate the User data to create the pending User data.
        pPendingData = _AllocThreadData(NULL, DEF_TIMEOUT, NULL, pThreadData->pCallBack,
                                        pThreadData->pPrivate);
        if (pPendingData == NULL)
        {
            DEBUG_LOG("%s\n", "Duplicating user data in thread failed. Deadlock!!");
            break;
        }

        // Ownership of pThreadData remains with 'parent' thread.
        result = dbus_connection_send_with_reply(pThreadData->pConn, pThreadData->pSharedData->pMsg,
                                                 &pPendingCall, pThreadData->timeout_milliseconds);
        if (!result || !pPendingCall)
        {
            DEBUG_LOG("%s\n", "client_lib: SendAsync failed.");
            break;
        }

        if (!pPendingData->pCallBack)
            dbus_pending_call_cancel(pPendingCall);
        else
            dbus_pending_call_set_notify(pPendingCall, _IpcHandleAsyncReply, pPendingData,
                                         _FreeThreadData);

        // Set the values for parent thread to allow cancellation or further async action if needed.
        if (_CreateClientCallHandle(&pThreadData->pSharedData->pCallHandle,
                                    pThreadData->pSharedData->szCallHandlePrefix,
                                    dbus_message_get_serial(pThreadData->pSharedData->pMsg),
                                    pPendingCall) != 0)
        {
            dbus_pending_call_cancel(pPendingCall);
            break;
        }

        UnblockOnSent(pThreadData->pSharedData);
        // WARNING: Do not use pThreadData beyond this point, as parent thread may destroy it.

        dbus_pending_call_block(pPendingCall);

        iSentFailed = 0;
    }
    while(0);

    if (iSentFailed && pThreadData)
    {
        DEBUG_LOG("%s\n", "Error in send messsage worker. Unblocking..");
        UnblockOnSent(pThreadData->pSharedData);
    }

    DEBUG_LOG("%s\n", "Finished send message worker.");
    return NULL;
}


/**
 * Requests the Security framework's IPC server asynchronously.
 */
int TSCSendMessageAsync(IpcClientInfo *pInfo, const char *service_name, const char *szMethod,
                        int argc, char **argv, TSC_CALL_HANDLE *pCallHandle, TSCCallback pCallback,
                        void *pPrivate, int timeout_milliseconds)
{
    DEBUG_LOG("%s %s\n", "sendmessage async ", service_name);
    DBusMessage *pMsg = NULL;
    DBusMessageIter dbiter;
    DBusError dberr;
    ThreadData *pThreadData = NULL;
    int i = 0;
    int iErr = TSC_ERROR_MODULE_GENERIC;

    // TODO: Avoid multiple exits.
    if (pInfo == NULL)
        return TSC_ERROR_INVALID_HANDLE;

    if (pInfo->dbconn == NULL)
        return TSC_ERROR_INVALID_HANDLE;

    if (argc < 0 || !szMethod)
        return TSC_ERROR_INVALID_PARAM;

    dbus_error_init(&dberr);
    pMsg = dbus_message_new_method_call(service_name, TSC_DBUS_PATH, TSC_DBUS_INTERFACE, szMethod);

    if (pMsg == NULL)
    {
        iErr = TSC_ERROR_INSUFFICIENT_RES;
        goto err_send;
    }

    dbus_message_iter_init_append(pMsg, &dbiter);

    // Add the message unique id as first element (the prefix).
    char *szIdPrefix = _GetMsgIdPrefix();
    if (!szIdPrefix || !dbus_message_iter_append_basic(&dbiter, DBUS_TYPE_STRING, &szIdPrefix))
    {
        DEBUG_LOG("client_lib: Failed to append id prefix for %s.\n", szMethod);
        free(szIdPrefix);
        goto err_send;
    }
    free(szIdPrefix);
    for (i = 0; i < argc; i++)
    {
        if (!dbus_validate_utf8(argv[i], &dberr))
        {
            DEBUG_LOG("%s", "client_lib: Not valid utf8 string\n");
            goto err_send;
        }
        if (!dbus_message_iter_append_basic(&dbiter, DBUS_TYPE_STRING, &argv[i]))
        {
            DEBUG_LOG("client_lib: %s failed to append arguments\n", pInfo->pid);
            goto err_send;
        }
    }

    // Reply is discarded and not sent back to client, if no callback available.
    if (!pCallback)
        dbus_message_set_no_reply(pMsg, TRUE);

    SharedData *pSharedData = _CreateSharedData(szIdPrefix, pMsg);
    if (!pSharedData)
    {
        iErr = TSC_ERROR_INSUFFICIENT_RES;
        goto err_send;
    }

    if (timeout_milliseconds == 0)
        timeout_milliseconds = DBUS_TIMEOUT_INFINITE;

    pThreadData = _AllocThreadData(pInfo->dbconn, timeout_milliseconds, pSharedData, pCallback,
                                   pPrivate);
    if (!pThreadData)
    {
        _FreeSharedData(pSharedData);
        iErr = TSC_ERROR_INSUFFICIENT_RES;
        goto err_send;
    }

    // Assert that the message is ready to be used.
    if (pThreadData->pSharedData->iSent)
    {
        DEBUG_LOG("%s\n", "client_lib: Sent flag already set!!");
        goto err_send;
    }

    DEBUG_LOG("Before sending: %d\n", dbus_message_get_serial(pMsg));
    if (_RunDetachedThread(_SendMessageWorker, pThreadData) == -1)
    {
        DEBUG_LOG("%s\n", "client_lib: Running thread failed!!");
        goto err_send;
    }

    BlockTillSent(pThreadData->pSharedData);
    DEBUG_LOG("After sending: %d\n", dbus_message_get_serial(pMsg));

    if (pCallHandle)
    {
        *pCallHandle = pThreadData->pSharedData->pCallHandle;
        //TODO: should add here
        DEBUG_LOG("method unique  id :%s\n", (*pCallHandle)->idUnique);
        strncpy((*pCallHandle)->service_name, service_name, TSC_SERVER_NAME_LEN);
    }
    else
    {
        // The caller does not want to call special methods (cancel, get progress) later.
        _FreeClientCallHandle(pThreadData->pSharedData->pCallHandle);
        pThreadData->pSharedData->pCallHandle = NULL;
    }

    iErr = 0;

err_send:
    if (pMsg)
        dbus_message_unref(pMsg);

    if (pThreadData)
        _FreeThreadData(pThreadData);

    return iErr;
}

/**
 * Releases the asynchronous call handle.
 */
void TSCFreeSentMessageHandle(ClientCallHandle *pCallHandle)
{
    _FreeClientCallHandle(pCallHandle);
}

/**
 * Callback when reply is received from IPC server for asynchronous method call.
 */
static void _IpcHandleAsyncReply(DBusPendingCall *pPendingCall, void *pThreadData)
{
    int i = 0;
    int argc = 0;
    char **argv = NULL;
    char *pBuf = NULL;
    char *pArgItem = NULL;
    DBusError dberr;
    DBusMessageIter dbiter;
    DBusMessage *pMsg = NULL;
    int iErr = TSC_ERROR_MODULE_GENERIC;
    int iSize = TSC_REPLY_MSG_COUNT_AVERAGE;
    ThreadData *pData = (ThreadData *)pThreadData;

    argv = malloc(iSize * sizeof(char *));
    if (!argv)
    {
        iErr = TSC_ERROR_INSUFFICIENT_RES;
        goto reply_err;
    }

    dbus_error_init(&dberr);

    pMsg = dbus_pending_call_steal_reply(pPendingCall);
    if (pMsg == NULL)
    {
        DEBUG_LOG("%s\n", "client_lib: reply message is NULL");
        goto reply_err;
    }

    i = 0;
    if (!dbus_message_iter_init(pMsg, &dbiter))
    {
        DEBUG_LOG("%s\n", "client_lib: Async reply has no arguments");
        goto reply_err;
    }

    do
    {
        if (dbus_message_iter_get_arg_type(&dbiter) != DBUS_TYPE_STRING)
        {
            DEBUG_LOG("%s\n", "client_lib: Reply argument is not string");
            goto reply_err;
        }

        dbus_message_iter_get_basic(&dbiter, &pArgItem);
        if (!pArgItem)
        {
            DEBUG_LOG("%s\n", "client_lib: Failed getting string arg from reply");
            goto reply_err;
        }

        if (!(pBuf = strdup((const char*)pArgItem)))
        {
            goto reply_err;
        }
        argv[i++] = pBuf;

        if (i >= iSize)
        {
            iSize += TSC_REPLY_MSG_COUNT_AVERAGE;
            argv = realloc(argv, sizeof(char*) * iSize);
            if (!argv)
            {
                goto reply_err;
            }
        }
    } while (dbus_message_iter_has_next(&dbiter) && dbus_message_iter_next(&dbiter));

    argc = i;
    // Continue passing the returned values to callback.
    goto forward_reply;

reply_err:
    // Reset values being returned.
    while (i)
        free(argv[--i]);
    if (argv)
    {
        free(argv);
        argv = NULL;
    }
    argc = i;

forward_reply:
    if(pMsg)
        dbus_message_unref(pMsg);
    if (pPendingCall)
        dbus_pending_call_unref(pPendingCall);

    if (pData && pData->pCallBack)
        (*(pData->pCallBack))(pData->pPrivate, argc, argv);

    DEBUG_LOG("%s\n", "client_lib: Async Reply Completed.");
}


/**
 * Cancels an asynchronous request previously made to the Security framework's IPC server.
 * On success, releases the handle of the previously called asynchronous method.
 *
 * TODO: the ClientCallHandle
 * and TSC_CALL_HANDLE (typedef struct _ClientCallHandle * TSC_CALL_HANDLE;) is confusing
 */
int TSCCancelMessage(IpcClientInfo *pInfo, ClientCallHandle *pCallHandle)
{
    int argc_reply = 0;
    char **argv_reply = NULL;
    char *argv_req[1];
    int iResult = -1;

    do
    {
        DEBUG_LOG("%s\n", "client_lib: CANCELing.");
        if (!pInfo || !pCallHandle)
            break;

        DEBUG_LOG("%s\n", "prepare cancel");
        // Cancel the message call locally.
        dbus_pending_call_cancel(pCallHandle->pPendingCall);

        // Now request the server to abort the running of the method.
        argv_req[0] = pCallHandle->idUnique;
        DEBUG_LOG("cancel method unique id:%s\n", argv_req[0]);
        //TODO: need change here for cancel message

        TSC_CALL_HANDLE handle = NULL;
        iResult = TSCSendMessageAsync(pInfo, pCallHandle->service_name, TSC_FN_CANCELMETHOD, 1,
                                      argv_req, &handle, NULL, NULL, DEF_TIMEOUT);
        DEBUG_LOG("%s\n", "client_lib: Sent Cancel to server.");

        if (iResult == 0)
        {
            _FreeClientCallHandle(pCallHandle);

            // Ignore the returned values, if any.
            if (argv_reply)
            {
                while (argc_reply)
                    free(argv_reply[--argc_reply]);
                free(argv_reply);
            }
        }

    } while (0);

    return iResult;
}

static DBusHandlerResult _IpcClientMsgFilter(DBusConnection *dbconn, DBusMessage *dbmsg, void *data)
{
    IpcClientInfo *info = (IpcClientInfo*) data;
    if (dbus_message_is_signal(dbmsg, DBUS_INTERFACE_LOCAL, "Disconnected"))
    {
    	DEBUG_LOG("client_lib: %s disconnected by signal\n", info->pid);
        info->dbconn = NULL;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static bool _IpcClientInit(IpcClientInfo *pInfo)
{
    if (!pInfo)
        return false;

    DBusError dberr;
    int ret;

    dbus_error_init(&dberr);

    dbus_threads_init_default();

    if (pInfo->dbconn != NULL)
        return false;

    pInfo->dbconn = dbus_bus_get(DBUS_BUS_SYSTEM, &dberr);

    if (dbus_error_is_set(&dberr))
    {
    	DEBUG_LOG("client_lib: %s error in get connection %s\n", pInfo->pid, dberr.message);
        goto err_get;
    }

    if (!pInfo->dbconn)
    {
    	DEBUG_LOG("client_lib: %s get connection is NULL\n", pInfo->pid);
        goto err_get;
    }

    dbus_connection_set_exit_on_disconnect(pInfo->dbconn, false);

    if (!dbus_connection_add_filter(pInfo->dbconn, _IpcClientMsgFilter, pInfo, NULL))
    {
    	DEBUG_LOG("client_lib: %s failed to add filter %s\n", pInfo->pid, dberr.message);
        goto err_get;
    }

    ret = dbus_bus_request_name(pInfo->dbconn, pInfo->req_name, DBUS_NAME_FLAG_REPLACE_EXISTING, &dberr);

    if (dbus_error_is_set(&dberr))
    {
    	DEBUG_LOG("client_lib: %s failed to request name %s\n", pInfo->pid, dberr.message);
        goto err_get;
    }

    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
    	DEBUG_LOG("client_lib: %s failed, it is not primary owner %d\n", pInfo->req_name, ret);
        goto err_get;
    }

    dbus_error_free(&dberr);
    return  true;

err_get:
    if (pInfo->dbconn)
    {
        dbus_connection_unref(pInfo->dbconn);
        pInfo->dbconn = NULL;
    }
    dbus_error_free(&dberr);

    return false;
}

static void _IpcClientDeInit(IpcClientInfo *pInfo)
{
    if (!pInfo)
        return;

    DBusError dberr;

    if (!pInfo->dbconn)
        return;

    dbus_error_init(&dberr);
    dbus_bus_release_name(pInfo->dbconn, pInfo->req_name, &dberr);
    if (dbus_error_is_set(&dberr))
    	DEBUG_LOG("client_lib: %s failed to release name %s\n", pInfo->pid, dberr.message);

    dbus_error_free(&dberr);

    dbus_connection_remove_filter(pInfo->dbconn, _IpcClientMsgFilter, pInfo);
    dbus_connection_unref(pInfo->dbconn);
    pInfo->dbconn = NULL;
}
