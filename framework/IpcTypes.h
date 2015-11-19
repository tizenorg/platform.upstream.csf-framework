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

#ifndef IPCTYPES_H
#define IPCTYPES_H

/**
 * \file IpcTypes.h
 * \brief Data types and structs for Security framework IPC clients.
 *
 * This file provides the data types and structs needed by clients of Security framework IPC.
 */

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/**
 * \brief CallBack Function type for Async method supported by the IPC.
 *
 * \param[in] pPrivate API caller's context information, supplied with TSCSendMessageAsync earlier.
 * \param[in] argc Length of the string in argv.
 * \param[in] argv Array of strings representing result value of asynchronous reply.
 */
typedef void (*TSCCallback)(void *pPrivate, int argc, const char **argv);

/**
 * Client side IPC handles.
 */
#define TSCHANDLE(n) struct n##_struct { int iDummy; }; typedef struct n##_struct *n

/**
 * IPC client handle.
 */
TSCHANDLE(TSC_IPC_HANDLE);
#define INVALID_IPC_HANDLE ((TSC_IPC_HANDLE) NULL)

/**
 * Asynchronous call handle.
 */
TSCHANDLE(TSC_CALL_HANDLE);

#endif  /* IPCTYPES_H */

