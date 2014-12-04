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
 * \file IpcServer.c
 * \brief Ipc Server Source File
 *
 * This file implements the IPC Server API functions used by Security framework.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "IpcMacros.h"
#include "IpcServerError.h"
#include "IpcServerHdr.h"
#include "TSCErrorCodes.h"


#ifdef DEBUG
#define DEBUG_LOG(_fmt_, _param_...)    \
    { \
        fprintf(stderr, "[TSC Server] %s %d " _fmt_, __FILE__, __LINE__, _param_); \
    }
#define DBUS_D_LOG(_dbusErr_, _fmt_, _param_...)    \
    { \
        DEBUG_LOG("%s:%s; " _fmt_, _dbusErr_.name, _dbusErr_.message, _param_); \
    }
#else
#define DEBUG_LOG(_fmt_, _param_...)
#define DBUS_D_LOG(_dbusErr_, _fmt_, _param_...)
#endif

static void IterateList(IpcMethodHandle* pHandle)
{
    IpcMethodHandle *ph;
    int count = 0;
    for(ph = pHandle; ph != NULL; ph = ph->pNext)
    {
        DEBUG_LOG(".....handle addr: %u, uniqueid:%s, name:%s\n", ph, ph->unique_id, ph->pMethod->szMethod);
        DEBUG_LOG("....pnext:%u\n", ph->pNext);
        count++;
    }
    DEBUG_LOG(".....total count: %d\n", count);
}

inline DBusHandlerResult _IpcSendMessageAndUnref(DBusConnection *pConn, DBusMessage *pMsg)
{
    if (pMsg) {
        dbus_connection_send(pConn, pMsg, NULL);
        dbus_message_unref(pMsg);
    }
    return DBUS_HANDLER_RESULT_HANDLED;
}

void _FreeHandleMethods(IpcServerInfo *pInfo)
{
    if (!pInfo)
        return;

    // Free MethodList
    IpcServerMethodList *pCurr = pInfo->pMethodList;
    IpcServerMethodList *pPrev = NULL;

    pthread_mutex_lock(&(pInfo->Lock));
    while (pCurr)
    {
        pPrev = pCurr;
        pCurr = pCurr->pNext;

        if (pPrev->pMethod)
        {
            /* TODO: Explain why its not leak for non-cancel methods.  */
            //if (pPrev->pMethod->method == (METHODFUNC)IpcCancelMethod || pPrev->pMethod->method == (METHODFUNC)IpcGetProgressMethod)
            free(pPrev->pMethod);

            pPrev->pMethod = NULL;
        }
        free(pPrev);
    }
    pInfo->pMethodList = NULL;
    pInfo->pRunningMethods = NULL;
    pthread_mutex_unlock(&(pInfo->Lock));

}

void _FreeHandlePool(IpcServerInfo *pInfo)
{
    if (!pInfo)
        return;

    while(IpcThrPoolIdleCount(pInfo->pHandlePool) != TSC_THREAD_POOL_NUMBERS && IpcThrPoolIdleCount(pInfo->pHandlePool) != -1)  //wait till no detachable thread is running
    {
        // TODO: Use condition instead of infinite loop
        sleep(1);
    }
    pthread_mutex_lock(&(pInfo->Lock));

    if (pInfo->pHandlePool)
    {
        IpcThrPoolFree(&pInfo->pHandlePool);
    }

    pthread_mutex_unlock(&(pInfo->Lock));
}

void _FreeHandleTable(IpcServerInfo *pInfo)
{
    if (!pInfo)
        return;

    pthread_mutex_lock(&(pInfo->Lock));
    if (pInfo->pTable)
    {
        free(pInfo->pTable);
        pInfo->pTable = NULL;
    }
    pthread_mutex_unlock(&(pInfo->Lock));
}

void _FreeHandleConn(IpcServerInfo *pInfo)
{
    if (!pInfo)
        return;

    pthread_mutex_lock(&(pInfo->Lock));
    if (pInfo->pConn)
    {
        DBusError dberr;
        dbus_error_init(&dberr);

        dbus_connection_remove_filter(pInfo->pConn, _IpcServerMsgFilter, pInfo);
        if (dbus_error_is_set(&dberr))
        {
            DEBUG_LOG("FreeHandleConn, remove_filter wrong:%s\n", dberr.message);
        }
	    pInfo->pConn = NULL;

    }
    pthread_mutex_unlock(&(pInfo->Lock));

}

void _WaitForListenThreadClose(IpcServerInfo *pInfo)
{
    if (pInfo && pInfo->lDbus_listen_thread)
    {
        pthread_mutex_lock(&(pInfo->Lock));
        pInfo->start_server_flag = false;
        pthread_mutex_unlock(&(pInfo->Lock));

        if (pInfo->lDbus_listen_thread)
            pthread_join(pInfo->lDbus_listen_thread, NULL);
    }
}

int IpcServerAddMethod(TSC_SERVER_HANDLE hServer, IpcServerMethod *pMethod)
{
    IpcServerInfo *pInfo = NULL;
    IpcServerMethodList *pList = NULL;
    int r = TSC_ERROR_ADD_METHOD_FAILED;

    if (INVALID_TSC_SERVER_HANDLE == hServer)
        return r;

	pList = calloc(1, sizeof(IpcServerMethodList));
	if (!pList)
	    return r;

	IpcServerMethod* tpMethod = calloc(1, sizeof(IpcServerMethod));
	if (tpMethod == NULL)
	{
	   free(pList);
	   return -1;
	}
	// Copy method

	tpMethod->method = pMethod->method;
	strncpy(tpMethod->szMethod, pMethod->szMethod, TSC_METHOD_NAME_LEN-1);
	tpMethod->pData = pMethod->pData;

    pInfo = (IpcServerInfo *) hServer;

    pthread_mutex_lock(&(pInfo->Lock));
    pList->pNext = pInfo->pMethodList;

    //pList->pMethod = pMethod;
    pList->pMethod = tpMethod;

    pInfo->pMethodList = pList;
    pthread_mutex_unlock(&(pInfo->Lock));

    r = 0;
    return r;
}

int IpcServerRemoveMethod(TSC_SERVER_HANDLE hServer, METHODFUNC method)
{
    IpcServerInfo *pInfo = NULL;
    IpcServerMethodList **pPrev = NULL;
    IpcServerMethodList *pCurr = NULL;
    int r = TSC_ERROR_REMOVE_METHOD_NOT_FOUND;

    DEBUG_LOG("%s\n", "IpcServerRemoveMethod");
    if (INVALID_TSC_SERVER_HANDLE == hServer)
        return r;

    pInfo = (IpcServerInfo *) hServer;

    // Change the head to next node, if deleted.
    pthread_mutex_lock(&(pInfo->Lock));
	for (pPrev = &pInfo->pMethodList; *pPrev; pPrev = &(*pPrev)->pNext)
	{
		DEBUG_LOG("REMOVE method list name :%s\n", (*pPrev)->pMethod->szMethod);
		if ((*pPrev)->pMethod->method == method)
		{
			DEBUG_LOG("==== FIND REVMOE MOETHOD %s\n", " ");
			pCurr = *pPrev;
			*pPrev = (*pPrev)->pNext;
            r = 0;
            break;
		}
	}
    pthread_mutex_unlock(&(pInfo->Lock));

    // Release the found method now.
    if (r == 0 && pCurr->pMethod)
    {
        free(pCurr->pMethod);
        pCurr->pMethod = NULL;
    }
    free(pCurr);

    return r;
}

TSC_SERVER_HANDLE IpcServerOpen(char *service_name)
{
    DEBUG_LOG("IpcServerOpen: %s\n", service_name);
	DBusError dberr;
	dbus_error_init(&dberr);

	dbus_threads_init_default();

    dbus_validate_bus_name(service_name, &dberr);
    if (dbus_error_is_set(&dberr))
    {
        DEBUG_LOG("it is invalid request name %s\n", "");
        goto err;
    }

	IpcServerInfo *pInfo;
	if ((pInfo = (IpcServerInfo *) calloc(1, sizeof(IpcServerInfo))) == NULL)
	    goto err;

	pInfo->pMethodList = NULL;
	pInfo->count = 0;
	if ((pInfo->pTable = (DBusObjectPathVTable *) calloc(1, sizeof(DBusObjectPathVTable))) == NULL)
	    goto free_info;

	pInfo->lDbus_listen_thread = 0;
	strncpy(pInfo->name, service_name, TSC_SERVER_NAME_LEN - 1);

	if (_IpcServerInit(pInfo, service_name))
	{
	    DEBUG_LOG("IpcServerInit failed: %s\n", service_name);
	    goto free_table;
	}

    //DEBUG_LOG("IpcServerOpen  success conn:%s\n", pInfo->name);
    dbus_error_free(&dberr);
    return (TSC_SERVER_HANDLE) pInfo;

free_table:
    SAFE_FREE(pInfo->pTable);

free_info:
    DEBUG_LOG("IpcServerOpen free_info, conn:%s\n", pInfo->name);
    FREE(pInfo);

err:
    dbus_error_free(&dberr);
    return INVALID_TSC_SERVER_HANDLE;
}

int IpcServerMainLoop(TSC_SERVER_HANDLE hServer)
{
    IpcServerInfo *pInfo = (IpcServerInfo*) hServer;
    if (pInfo)
    {
        if (pInfo->lDbus_listen_thread)
        {
            pthread_join(pInfo->lDbus_listen_thread, NULL);
            DEBUG_LOG("finsihed main loop:%s\n", "==========");
        }

    }
    return 0;
}
void IpcServerClose(TSC_SERVER_HANDLE *hServer)
{
	IpcServerInfo *pInfo = (IpcServerInfo *) *hServer;
	if (pInfo != (IpcServerInfo*)INVALID_TSC_SERVER_HANDLE)
	{
	    DEBUG_LOG("IpcServerClose:%s\n", pInfo->name);
	    _WaitForListenThreadClose(pInfo);

	    // Wait till no detachable thread is running
        while (IpcThrPoolIdleCount(pInfo->pHandlePool) != TSC_THREAD_POOL_NUMBERS && IpcThrPoolIdleCount(pInfo->pHandlePool) != -1)
        {
            // TODO: Use conditional mutex.
            sleep(1);
        }

        _FreeHandleTable(pInfo);
        _FreeHandleMethods(pInfo);
        _FreeHandlePool(pInfo);
        _FreeHandleConn(pInfo);
        pthread_mutex_destroy(&(pInfo->Lock));

        free(pInfo);
        *hServer = INVALID_TSC_SERVER_HANDLE;
	}
}


int _IpcServerInit(IpcServerInfo *pInfo, char *szServiceName)
{
    IpcServerMethod *pMethodCancel = NULL;
    DBusError dberr;
    int ret;

    if (!pInfo)
        goto err;

    pInfo->pMethodList = NULL;
    pInfo->pRunningMethods = NULL;
    pInfo->start_server_flag = false;

    dbus_error_init(&dberr);

    pInfo->pConn = dbus_bus_get(DBUS_BUS_SYSTEM, &dberr);
    if (dbus_error_is_set(&dberr))
    {
        DBUS_D_LOG(dberr, "%s\n", "Server failed: connection NULL.");
        goto free_err;
    }

    ret = dbus_bus_request_name(pInfo->pConn, szServiceName, DBUS_NAME_FLAG_REPLACE_EXISTING, &dberr);
    if (dbus_error_is_set(&dberr))
    {
        DBUS_D_LOG(dberr, "%s\n", "server failed: request name.");
        goto free_conn;
    }

    // TODO: Why not allow DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER also?
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
    	DEBUG_LOG("server failed: Not primary owner :%d\n", ret);
        goto free_conn;
    }

    if (0 > snprintf(pInfo->rule, sizeof(pInfo->rule), "type='method_call', interface='%s'",
                     TSC_DBUS_INTERFACE))
    {
        DEBUG_LOG("%s\n", "server failed: Unable to write rule");
        goto free_conn;
    }

    dbus_bus_add_match(pInfo->pConn, pInfo->rule, &dberr);
    if (dbus_error_is_set(&dberr))
    {
        DBUS_D_LOG(dberr, "%s\n", "Match add failed.");
        goto free_conn;
    }

    ret = dbus_connection_try_register_object_path(pInfo->pConn, TSC_DBUS_PATH, pInfo->pTable,
                                                   pInfo, &dberr);
    if (dbus_error_is_set(&dberr) || !ret)
    {
        DBUS_D_LOG(dberr, "%s\n", "server failed: register object path");
        goto free_match;
    }

    dbus_connection_set_exit_on_disconnect(pInfo->pConn, false);

    if (!dbus_connection_add_filter(pInfo->pConn, _IpcServerMsgFilter, pInfo, NULL))
    {
    	DEBUG_LOG("%s\n", "server failed: add filter.");
        goto free_register;
    }

    if (!dbus_connection_get_unix_fd(pInfo->pConn, &pInfo->fd) || pInfo->fd < 0)
    {
    	DEBUG_LOG("%s\n", "server failed: get fd.");
        goto free_filter;
    }

    pInfo->pHandlePool = calloc(1, sizeof(IpcHandlePool));
    if (pInfo->pHandlePool == NULL)
    {
        DEBUG_LOG("%s\n", "Thread pool alloc failed.");
        goto free_filter;
    }

    ret = IpcThrPoolInit(pInfo->pHandlePool, TSC_THREAD_POOL_NUMBERS);
    if (ret)
    {
        DEBUG_LOG("%s\n", "*****Failed in IpcThrPoolInit");
        goto free_handle;
    }

    ret = pthread_mutex_init(&(pInfo->Lock), NULL);
    if (ret)
    {
        DEBUG_LOG("Failed to init IpcServerInfo lock %d\n", ret);
        goto free_pool;
    }

    // Add default two methods: Cancel and GetStatus
    pMethodCancel = calloc(1, sizeof(IpcServerMethod));
    if (pMethodCancel == NULL)
    {
        DEBUG_LOG("%s\n", "Cancel alloc failed.");
        goto free_mutex;
    }

    ret = snprintf(pMethodCancel->szMethod, sizeof(pMethodCancel->szMethod), "%s", TSC_FN_CANCELMETHOD);
    if (ret < 0)
    {
        DEBUG_LOG("%s\n", "Cancel create failed.");
        goto free_method_cancel;
    }

    pMethodCancel->method = (METHODFUNC) IpcCancelMethod;
    pMethodCancel->pData = NULL;
    ret = IpcServerAddMethod((TSC_SERVER_HANDLE) pInfo,  pMethodCancel);
    if (ret)
    {
        DEBUG_LOG("%s\n", "Cancel add failed.");
        goto free_method_cancel;
    }
    FREE(pMethodCancel);


    IpcServerMethod *pMethodProgress = calloc(1, sizeof(IpcServerMethod));
    if (pMethodProgress == NULL)
    {
        DEBUG_LOG("%s\n", "Progress alloc failed.");
        goto free_method_cancel;
    }

    ret = snprintf(pMethodProgress->szMethod, sizeof(pMethodProgress->szMethod), "%s", TSC_FN_PROGRESSMETHOD);
    if (ret < 0)
    {
        DEBUG_LOG("%s\n", "Progress create failed.");
        goto free_method_progress;
    }

    pMethodProgress->method = (METHODFUNC) IpcGetProgressMethod;
    pMethodProgress->pData = NULL;

    ret = IpcServerAddMethod((TSC_SERVER_HANDLE) pInfo, pMethodProgress);
    if (ret)
    {
        DEBUG_LOG("%s\n", "Progress add failed.");
        goto free_method_progress;
    }
    FREE(pMethodProgress);


    IpcServerMethod *pMethodShutdown = calloc(1, sizeof(IpcServerMethod));
    if (pMethodShutdown == NULL)
    {
        DEBUG_LOG("%s\n", "shutdown alloc failed.");
        goto free_method_progress;
    }

    ret = snprintf(pMethodShutdown->szMethod, sizeof(pMethodShutdown->szMethod), "%s", TSC_FN_SHUTDOWN);
    if (ret < 0)
    {
        DEBUG_LOG("%s\n", "Shutdown create failed.");
        goto free_method_shutdown;
    }

    pMethodShutdown->method = (METHODFUNC) IpcShutdown;
    pMethodShutdown->pData = NULL;

    ret = IpcServerAddMethod((TSC_SERVER_HANDLE) pInfo, pMethodShutdown);
    if (ret)
    {
        DEBUG_LOG("%s\n", "Shutdown add failed.");
        goto free_method_shutdown;
    }
    FREE(pMethodShutdown);

    pthread_mutex_lock(&(pInfo->Lock));

    ret = pthread_create(&(pInfo->lDbus_listen_thread), NULL, _IpcPopMessage, (void *)pInfo);
    DEBUG_LOG("Creating thrd for Server: %s\n", pInfo->name);

    if (ret)
    {
        DEBUG_LOG("%s(%d)\n", "** FAILED to launch thread", ret);
        pInfo->start_server_flag = false;
        pthread_mutex_unlock(&(pInfo->Lock));
        goto free_mutex;
    }
    pInfo->start_server_flag = true;

    pthread_mutex_unlock(&(pInfo->Lock));

    return 0;
free_method_shutdown:
    FREE(pMethodShutdown);
free_method_progress:
    FREE(pMethodProgress);
free_method_cancel:
    FREE(pMethodCancel);
free_mutex:
    pthread_mutex_destroy(&(pInfo->Lock));
free_pool:
free_handle:
    if (pInfo->pHandlePool)
        IpcThrPoolFree(&pInfo->pHandlePool);
free_filter:
    //dbus_connection_remove_filter(pInfo->pConn, _IpcServerMsgFilter, pInfo);
free_register:
    //dbus_connection_unregister_object_path(pInfo->pConn, TSC_DBUS_PATH);
free_match:
    //dbus_bus_remove_match(pInfo->pConn, pInfo->rule, &dberr);
free_name:
    //dbus_bus_release_name(pInfo->pConn, szServiceName, &dberr);
free_conn:
    //dbus_connection_close(pInfo->pConn); TODO: Why not????
free_err:
    dbus_error_free(&dberr);
    DEBUG_LOG("%s", "_IpcServerInit free_err before return -1\n");
err:
    return -1;
}

void _IpcServerDeInit(TSC_SERVER_HANDLE hServer)
{
    DEBUG_LOG("%s\n", "IpcServerDeInit========");
	IpcServerInfo *pInfo = (IpcServerInfo *) hServer;
    if (pInfo)
    {
        if (pInfo->lDbus_listen_thread)
        {
            pthread_join(pInfo->lDbus_listen_thread, NULL);
        }
        FreeIpcServerHandle((TSC_SERVER_HANDLE) pInfo);
    }
    //dbus_shutdown(); // for valgrind only
    DEBUG_LOG("%s\n", "*_*_*_*_*_*_* Server is disconnected *_*_*_*_*_*_*_*_*");
}


DBusHandlerResult _IpcServerReplyMessage(DBusConnection *pConn, DBusMessage *pMsg,
                                                char **pReply, int size)
{
    DEBUG_LOG("%s %d\n", "ReplyMessage size: ", size);
    DBusMessage *pReplyMsg = NULL;
    DBusMessageIter    iter;
    int i;

    if (pConn == NULL)
        return DBUS_HANDLER_RESULT_HANDLED;

    pReplyMsg = dbus_message_new_method_return(pMsg);

    if (pReplyMsg == NULL)
        return DBUS_HANDLER_RESULT_NEED_MEMORY;

    dbus_message_iter_init_append(pReplyMsg, &iter);

    for (i = 0; i < size; i++)
    {
        if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &(pReply[i])))
        {
            DEBUG_LOG("----- Replied string :%s\n", pReply[i]);
            dbus_message_unref(pReplyMsg);
            return DBUS_HANDLER_RESULT_NEED_MEMORY;
        }
    }

    return _IpcSendMessageAndUnref(pConn, pReplyMsg);
}

DBusHandlerResult _IpcServerReplyError(DBusConnection *pConn, DBusMessage *pMsg,
                                              int iErrCode)
{
    DEBUG_LOG("%s\n", "IpcServerReplyError");
    DBusMessage *pErrMsg = NULL;

    if (!pConn || !pMsg)
        return DBUS_HANDLER_RESULT_HANDLED;

    pErrMsg = dbus_message_new_error(pMsg, GetErrorName(iErrCode), GetErrorDescription(iErrCode));
    if (!pErrMsg)
        return DBUS_HANDLER_RESULT_NEED_MEMORY;

    return _IpcSendMessageAndUnref(pConn, pErrMsg);
}


DBusHandlerResult _IpcServerMsgFilter(DBusConnection *pConn, DBusMessage *pMsg, void *pData)
{
    DEBUG_LOG("%s\n", "IpcServerMsgFilter");
    IpcServerInfo *pInfo = (IpcServerInfo*) pData;
    if (!pInfo)
    {
    	DEBUG_LOG("%s\n", "not handled the server info");
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
    else if (dbus_message_is_signal(pMsg, DBUS_INTERFACE_LOCAL, "Disconnected"))
    {
    	DEBUG_LOG("%s\n", "server is disconnected by signal");
        IpcServerClose((TSC_SERVER_HANDLE*) pInfo);
        //return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


DBusHandlerResult _IpcServerMsgHandler(void *user_data)
{
    DBusHandlerResult ret = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    IpcAsyncInfo *pAsync = user_data;
    DBusConnection *pConn = pAsync->pConn;
    DBusMessage *pMsg = pAsync->pMsg;
    IpcServerInfo *pInfo = pAsync->pInfo;


    bool handle_flag = false;
    if (pInfo)
    {
        DEBUG_LOG("==IpcServerMsgHandler:%s\n", pInfo->name);
        IpcServerMethodList **pPrev;
        pthread_mutex_lock(&(pInfo->Lock));
        for (pPrev = &pInfo->pMethodList; *pPrev; pPrev = &(*pPrev)->pNext)
        {
            if ((*pPrev) && (*pPrev)->pMethod)
            {
                if (dbus_message_is_method_call(pMsg, TSC_DBUS_INTERFACE, (*pPrev)->pMethod->szMethod)){
                    DEBUG_LOG("FOUND method: %s\n", (*pPrev)->pMethod->szMethod);
                    handle_flag = true;
                    pAsync->pMethod = (*pPrev)->pMethod;
                    break;
                }
            }
        }
        pthread_mutex_unlock(&(pInfo->Lock));
        if (handle_flag)
        {
            return _IpcServerProcessMessage(pAsync);
        }
    }
    if (ret == DBUS_HANDLER_RESULT_NOT_YET_HANDLED)
    {
        if (pAsync && pAsync->pInfo && pAsync->pInfo->pHandlePool)
        {
            IpcThrPoolPut(pAsync->pInfo->pHandlePool, pAsync->pHandle);
        }
        CleanupAsync(pAsync);
    }

    return ret;
}

int _ParseDBusMessage(DBusMessage *pMsg, int *pargc, char ***argv)
{
    DBusBasicValue temp = {0};
    int argc = 0;
    int iSize = TSC_REPLY_MSG_COUNT_AVERAGE;
    DBusMessageIter iter;
    int iRet = TSC_ERROR_NOT_IMPLEMENTED;   // Return as result of method requested by client.

    if (dbus_message_iter_init(pMsg, &iter))
    {
        do
        {
            if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING)
            {
                DEBUG_LOG("%s\n", "wrong type");
                if (argc == 0)
                    iSize = 0;
                iRet = TSC_ERROR_NOT_IMPLEMENTED;
                goto clean_up;
            }
            dbus_message_iter_get_basic(&iter, &temp);
            if (!temp.str)
            {
                DEBUG_LOG("%s\n", "no string");
                iRet = TSC_ERROR_INTERNAL;
                goto clean_up;
            }

            if (argc >= iSize)
            {
                iSize += TSC_REPLY_MSG_COUNT_AVERAGE;
                (*argv) = realloc((*argv), sizeof(char*) * iSize);

                if (!(*argv))
                {
                    DEBUG_LOG("PARSE message, insufficient: 7-1, argc:%d iSize:%d\n",  argc, iSize);
                    iRet = TSC_ERROR_INSUFFICIENT_RES;
                    iSize = iSize - TSC_REPLY_MSG_COUNT_AVERAGE;
                    goto clean_up;
                }
            }
            else if (argc == 0)
            {
                (*argv) = calloc(1, iSize * sizeof(char *));
                if (!(*argv))
                {
                    iRet = TSC_ERROR_INSUFFICIENT_RES;
                    iSize = 0;  //reset iSize, used to free the memory
                    goto clean_up;
                }
            }

            (*argv)[argc] = strdup((const char*)temp.str);

            if (!(*argv)[argc])
            {
                DEBUG_LOG("not engough memory: %s\n", "4");
            	 iRet = TSC_ERROR_INSUFFICIENT_RES;
            	 goto clean_up;
            }
            argc++;
        } while (dbus_message_iter_next(&iter));

        iRet = 0;   // TODO: Is this the right place & code?
    }
    else
    {
        DEBUG_LOG("%s\n", "Request message has no arguments");
        *pargc = argc;
        return iRet;
    }

clean_up:

    if (iRet)
    {
        DEBUG_LOG("ADDRESS to clean:%u\n", *argv);
        //Error code here
    	int i = 0;
    	for (i = 0; i < iSize; i++)
    	{
    		if ((*argv)[i])
    			free((*argv)[i]);
    		(*argv)[i] = NULL;
    	}
		if (*argv)
		    free(*argv);
        *argv = NULL;
    }
    else
    {
        // Everything went well till now...
        *pargc = argc;
        int i = 0;


        for (i = 0; i < argc; i++)
            DEBUG_LOG("thread:%u, argc:%d, i:%d, Parsemessage :msg:%s\n", pthread_self(),argc, i,
                      (*argv)[i]);
    }

    return iRet;
}


DBusHandlerResult _IpcServerProcessMessage(void *user_data)
{
    DEBUG_LOG("%s\n", "IpcServerProcessMessage");
    IpcAsyncInfo *pAsync = user_data;
    DBusError dberr;
    DBusMessageIter iter;
    int iSize = TSC_REPLY_MSG_COUNT_AVERAGE;
    char **reply = NULL;
    int iFreeMtdHandle = 0;
    int len = 0;
//    int iErr = DBUS_HANDLER_RESULT_HANDLED; // Return the result to caller of this function.
    int iRet = TSC_ERROR_MODULE_GENERIC;   // Return as result of method requested by client.

    dbus_error_init(&dberr);
    if (pAsync->pConn == NULL)
    {
        goto clean_up;
    }
    // Here when calling method, pass the callback function
    IpcMethodHandle *pMtdHandle = calloc(1, sizeof(IpcMethodHandle));

    if (pMtdHandle == NULL)
    {
        goto clean_up;
    }

    pMtdHandle->pMethod = pAsync->pMethod;
    iRet = pthread_mutex_init(&(pMtdHandle->Lock), NULL);
    if (iRet)
    {
        if (pMtdHandle)
            free(pMtdHandle);
        pMtdHandle = NULL;
        goto clean_up;
    }

    if ((pAsync->pMethod)->method)
    {
        // insert to RunningMethods
        if (pAsync->argv[0] == NULL)
        {
            goto clean_up;
        }

        strncpy(pMtdHandle->unique_id, pAsync->async_unique_id, MSGHANDLE_LEN);

        if (pMtdHandle->unique_id == NULL)
        {
            DEBUG_LOG("unable to copy unique_id:%u\n", pthread_self());
            goto clean_up;
        }


        pthread_mutex_lock(&pAsync->pInfo->Lock);
        (pAsync->pInfo->count)++;
        pMtdHandle->pNext = pAsync->pInfo->pRunningMethods;
        pAsync->pInfo->pRunningMethods = pMtdHandle;
/*
        DEBUG_LOG("============== after adding running method:%s\n", "------");
        IterateList(pMtdHandle);
*/
        pthread_mutex_unlock(&pAsync->pInfo->Lock);

        pMtdHandle->pInfo = pAsync->pInfo;
        pMtdHandle->cStatus = "1";
        pMtdHandle->iCancel = TSC_NON_CANCEL;

        // Skip the first params which is for unique_id
        DEBUG_LOG("method to run:%s, argv[0]:%s\n", pAsync->pMethod->szMethod, pAsync->argv[0]);

        iRet = (pAsync->pMethod)->method((pAsync->pMethod)->pData, pAsync->argc - 1,
                                         &(pAsync->argv[1]), &reply, &len,
                                         (CALLBACKFUNC) IpcServerCallbackMethod, pMtdHandle);
        iRet = 0; //till here, able to run the method, it is success
    }

clean_up:

    if (iRet)
        _IpcServerReplyError(pAsync->pConn, pAsync->pMsg, TSC_ERROR_INTERNAL);
    else
    {
        // Everything went well till now...
        _IpcServerReplyMessage(pAsync->pConn, pAsync->pMsg, reply, len);
    }

    dbus_connection_flush(pAsync->pConn);
    CleanupArray(&reply, len);

    // Method finished, remove it from pInfo, running methods list
    IpcMethodHandle **pmpPrev;
    pthread_mutex_lock(&(pAsync->pInfo->Lock));
    for (pmpPrev = &(pAsync->pInfo->pRunningMethods); *pmpPrev; pmpPrev = &((*pmpPrev)->pNext))
    {
        if (strncmp((*pmpPrev)->unique_id, pAsync->async_unique_id,  MSGHANDLE_LEN) == 0)
        {
            *pmpPrev = (*pmpPrev)->pNext;
            iFreeMtdHandle = 1;
            DEBUG_LOG("FOUND method, and remove from running one, method:%s, uniquid:%s\n",
                      pAsync->pMethod->szMethod, pAsync->async_unique_id);
            IterateList(pAsync->pInfo->pRunningMethods);
            break;
        }
    }
    pthread_mutex_unlock(&(pAsync->pInfo->Lock));

    IpcThrPoolPut(pAsync->pInfo->pHandlePool, pAsync->pHandle);
    CleanupAsync(pAsync);
    dbus_error_free(&dberr);

    DEBUG_LOG(">>>>> after search, mtdHandleId:%s, iFreeFlag:%d, thread:%u, pMtdHandle:%u\n",
              pMtdHandle->unique_id, iFreeMtdHandle, pthread_self(), pMtdHandle);
    if (pMtdHandle && iFreeMtdHandle)
    {
        free(pMtdHandle);
        pMtdHandle = NULL;
    }

    return iRet;
}

void *_IpcPopMessage(void *hServer)
{
    IpcServerInfo *pInfo = (IpcServerInfo *) hServer;
    DEBUG_LOG("=IpcPopMessage:%s\n", pInfo->name);
    int iRet = 0;

    while (pInfo != NULL && pInfo->pConn != NULL && pInfo->start_server_flag) {

        // non blocking read of the next available message
        dbus_connection_read_write(pInfo->pConn, 0);
        DBusMessage *pMsg = dbus_connection_pop_message(pInfo->pConn);

        if (NULL == pMsg) {
            sleep(TSC_READ_WRITE_DISPATCH_SLEEP_SECONDS);
            continue;
        }
        else
        {
            // TODO: Strangely, first message needs to be processed, for the next N pending msgs
            // to be picked up for asynchronous processing.
            IpcHandles *pIpcHandle = IpcThrPoolGet(pInfo->pHandlePool);
        	IpcAsyncInfo *pAsync = calloc(1, sizeof(IpcAsyncInfo));
        	if (pAsync != NULL)
        	{
                pAsync->pConn = pInfo->pConn;
                pAsync->pInfo = pInfo;
                pAsync->pMsg = pMsg;
                pAsync->argc = 0;
                pAsync->argv = NULL;
                pAsync->pHandle = pIpcHandle;

                iRet = _ParseDBusMessage(pMsg, &(pAsync->argc), &(pAsync->argv));

                if (iRet == 0)
                {
                    iRet = snprintf(pAsync->async_unique_id, MSGHANDLE_LEN, TSC_MID_SVR_FORMAT, pAsync->argv[0],
                                                       dbus_message_get_serial(pAsync->pMsg));
                    //DEBUG_LOG("ASYNC_UNQIEU-DI: %s\n", pAsync->async_unique_id);
                    if (iRet < 0)
                        break;

                    pthread_t lthread;
                    iRet = _RunDetachedThread(_IpcServerMsgHandler, pAsync);
                    DEBUG_LOG("====RunDetachedThread ret:%d\n", iRet);
                }

                if (iRet)
                {
                    IpcThrPoolPut(pInfo->pHandlePool, pIpcHandle);
                    CleanupAsync(pAsync);
                }

        	}
        }
    }
    DEBUG_LOG("popmessage ended :%s\n", "============");

    return NULL;
}


void CleanupArray(char*** pArr, int len)
{
    if (pArr) {
        while (len > 0)
        {
            len--;
            free ((*pArr)[len]);
            (*pArr)[len] = NULL;
        }
        free(*pArr);
        *pArr = NULL;
    }
}

void CleanupAsync(IpcAsyncInfo *pAsync)
{
    if (pAsync)
    {
        CleanupArray(&(pAsync->argv), pAsync->argc);
        if (pAsync->pMsg)
            dbus_message_unref(pAsync->pMsg);

        if (pAsync)
            free(pAsync);
        pAsync = NULL;

    }
}

/***
 * reason_params is cleaned up within this method
 */
int IpcServerCallbackMethod(TSC_METHOD_HANDLE *pMHandle, TSC_METHOD_REASON_CODE iReason, void *reason_params)
{
    IpcMethodHandle  *p_MHandle = (IpcMethodHandle *) pMHandle;
    char *data = reason_params;
    int iRet = -1;

    if (iReason == TSC_CANCEL)
    {
        pthread_mutex_lock(&(p_MHandle->Lock));
        if (p_MHandle->iCancel == TSC_IS_CANCEL)
        {
            iRet = 0;
            DEBUG_LOG("%s", "callback check, it is cancel true\n");
        }
        pthread_mutex_unlock(&(p_MHandle->Lock));
    }
    else if (iReason == TSC_PROGRESS)
    {
        pthread_mutex_lock(&(p_MHandle->Lock));
        p_MHandle->cStatus = strdup(data);  // TODO: any better way to update data?
        iRet = 0;
        pthread_mutex_unlock(&(p_MHandle->Lock));

        if (data)
            free(data);
        data = NULL;
    }
    return iRet;
}

/*void IpcCancelMethod(TSC_SERVER_HANDLE hServer, char  *method_unique_id)*/
int IpcCancelMethod(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{

    int ret = 0;
    if (argc > 0)
    {
        DEBUG_LOG("IpcCancelMethod unique_id %s\n", argv[0]);
    }
    else
    {
        DEBUG_LOG("%s\n", "Error params in cancel method");
    }

    //TODO :error checking
    IpcMethodHandle *pMHandle = (IpcMethodHandle*) handle;
    IpcServerInfo *pInfo = (IpcServerInfo*) pMHandle->pInfo;
    // Get the running method by the method_unique_id, then set its cancel flag
    IpcMethodHandle **pmpPrev;

    if (argc != 1)
    {
        //the argc should be 2, the second param is unique id of to cancel method
        ret = -1;
    }
    pthread_mutex_lock(&(pInfo->Lock));

    for (pmpPrev = &(pInfo->pRunningMethods); *pmpPrev; pmpPrev = &((*pmpPrev)->pNext))
    {
        DEBUG_LOG("Method to cancel: %s, list method:%s\n", argv[0], (*pmpPrev)->unique_id);
        if (strcmp((*pmpPrev)->unique_id, argv[0]) == 0)
        {
            DEBUG_LOG("%s\n", "found the running method to cancel");
            pthread_mutex_lock(&(pMHandle->Lock));
            (*pmpPrev)->iCancel = TSC_IS_CANCEL;
            DEBUG_LOG("%s %s\n", "set is cancel to true for this method: ", (*pmpPrev)->unique_id);
            //TODO: should we return something?
            *len = 1;
            *szReply = calloc(1, sizeof(char*) *(*len));
            (*szReply)[0] = strdup("0");
            pthread_mutex_unlock(&(pMHandle->Lock));
            break;
        }
    }
    pthread_mutex_unlock(&(pInfo->Lock));

    DEBUG_LOG("%s\n", "END OF cancel method");
    return ret;
}



int
IpcGetProgressMethod(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    DEBUG_LOG("%s\n", "IpcGetProgressMethod");
    IpcMethodHandle *pMHandle = (IpcMethodHandle*) handle;
    IpcServerInfo *pInfo = (IpcServerInfo*) pMHandle->pInfo;

    //TODO: Error handling, check argv argc
    //Here the running method finished, either end or cancelled.
    IpcMethodHandle **pmpPrev;
    for (pmpPrev = &(pInfo->pRunningMethods); *pmpPrev; pmpPrev = &((*pmpPrev)->pNext))
    {
        DEBUG_LOG("running methods unique id :%s,  passing id :%s \n", (*pmpPrev)->unique_id, argv[0]);

        if (!strcmp((*pmpPrev)->unique_id, argv[0]))
        {
            DEBUG_LOG("=== found running method to get progress\n", argv[0]);
            // get the running method, update its status
            *len = 1;
            *szReply = calloc(1, sizeof(char*) * 10);
            DEBUG_LOG("-- status to reply :%s\n",(*pmpPrev)->cStatus);
            (*szReply)[0] = strdup((*pmpPrev)->cStatus);
            break;
        }
    }
    return 0;
}

int
IpcShutdown(void *pData, int argc, char **argv, char ***szReply, int *len, CALLBACKFUNC callback, TSC_METHOD_HANDLE *handle)
{
    DEBUG_LOG("==============%s\n", "IpcShutdownMethod");
    IpcMethodHandle *pMHandle = (IpcMethodHandle*) handle;
    IpcServerInfo *pInfo = (IpcServerInfo*) pMHandle->pInfo;

    pthread_mutex_lock(&(pInfo->Lock));
    pInfo->start_server_flag = false;
    pthread_mutex_unlock(&(pInfo->Lock));
    DEBUG_LOG("end of shutdown:%s\n", "===================");
    return 0;
}


void FreeIpcServerHandle(TSC_SERVER_HANDLE hServer)
{
    DEBUG_LOG("%s\n", "FreeIpcServerHandle");
    IpcServerInfo *pInfo = (IpcServerInfo*) hServer;

    // Free MethodList
    IpcServerMethodList *pCurr = pInfo->pMethodList;
    IpcServerMethodList *pPrev;

    while (pCurr)
    {
        pPrev = pCurr;
        pCurr = pCurr->pNext;
        DEBUG_LOG("*****pPrev method is 0 :%s\n", pPrev->pMethod->szMethod);

        if (pPrev)
        {
            if (pPrev->pMethod)
            {
                if (pPrev->pMethod->method == (METHODFUNC) IpcCancelMethod
                        || pPrev->pMethod->method == (METHODFUNC) IpcGetProgressMethod
                        || pPrev->pMethod->method == (METHODFUNC) IpcShutdown)
                    free(pPrev->pMethod);

                pPrev->pMethod = NULL;
            }
            if (pPrev->pNext)
            {
                pPrev->pNext = NULL;
            }
            free(pPrev);
            pPrev = NULL;
        }
    }

    pInfo->pMethodList = NULL;

    pInfo->pRunningMethods = NULL;

    if (pInfo->pHandlePool)
        IpcThrPoolFree(&pInfo->pHandlePool);

    pthread_mutex_destroy(&(pInfo->Lock));

    if (pInfo->pTable)
        free(pInfo->pTable);
    pInfo->pTable = NULL;

    if (pInfo->pConn)
    {
        DBusError dberr;
        dbus_error_init(&dberr);

        dbus_connection_remove_filter(pInfo->pConn, _IpcServerMsgFilter, pInfo);
        dbus_connection_unregister_object_path(pInfo->pConn, TSC_DBUS_PATH);
        dbus_bus_remove_match(pInfo->pConn, pInfo->rule, &dberr);

        dbus_bus_release_name(pInfo->pConn, TSC_DBUS_SERVER, &dberr);

        dbus_connection_unref(pInfo->pConn);
        dbus_error_free(&dberr);
    }
    pInfo->pConn = NULL;

    if (pInfo)
        free(pInfo);
    pInfo = NULL;
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

