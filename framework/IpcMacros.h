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

#ifndef IPCMACROS_H
#define IPCMACROS_H

/**
 * \file IpcMacros.h
 * \brief Common constants and macros Header File
 *  
 * This file provides the constants and macros for IPC used by Security framework.
 */

#define TSC_DBUS_SERVER_GENERIC_STUB "com.tsc.ipc.server.generic"
#define TSC_DBUS_SERVER_WP_CHANNEL "com.tsc.ipc.server.wp"
#define TSC_DBUS_SERVER_PLUGIN_CHANNEL "com.tsc.ipc.server.plugin"
#define TSC_DBUS_SERVER_INTEGRITY_TEST_STUB "com.tsc.ipc.server.integrity"

#define TSC_DBUS_SERVER        "com.tsc.ipc.server"
#define TSC_DBUS_INTERFACE     "org.tsc.ipc.interface"
#define TSC_DBUS_PATH          "/org/tsc/ipc/path"
#define TSC_DBUS_CLIENT        "com.tsc.ipc.client"
#define TSC_FN_FETCHLIST       "FetchList"
#define TSC_FN_CANCELMETHOD    "_Cancel_"
#define TSC_FN_PROGRESSMETHOD  "_Progress_"
#define TSC_FN_SHUTDOWN "IpcShutdown"

#define TSC_MID_PREFIX_FORMAT  "%u_%lu_"
#define TSC_MID_FORMAT         TSC_MID_PREFIX_FORMAT"%u"
#define TSC_MID_SVR_FORMAT     "%s%u"

#define TSC_READ_WRITE_DISPATCH_SLEEP_SECONDS	1
#define TSC_SEND_MESSAGE_REPLY_TIME 	5000

#define TSC_THREAD_POOL_NUMBERS 3

#define TSC_REQ_STR_LEN		128
#define TSC_INFO_RULE_LEN 	128
#define TSC_SERVER_NAME_LEN 128
#define TSC_METHOD_NAME_LEN 128

#define TSC_REPLY_MSG_COUNT_AVERAGE 4


// Client side call handle length.
// TODO: Reduce size by avoiding string type handle.
#define MSGHANDLE_LEN   25

typedef enum
{
    TSC_CANCEL = -1,
    TSC_PROGRESS = 0
} TSC_METHOD_REASON_CODE;

#define TSC_IS_CANCEL   0
#define TSC_NON_CANCEL   1;

#endif  /* IPCMACROS_H */

