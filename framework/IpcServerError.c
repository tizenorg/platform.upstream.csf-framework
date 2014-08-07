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
 * \file IpcServerError.c
 * \brief Ipc server-side error handling File
 *  
 * This file contains the server-side exception handling, used by Security framework.
 */

#include "IpcMacros.h"
#include "IpcServerError.h"
#include "TSCErrorCodes.h"

const char *GetErrorName(const int iErrCode)
{
    switch (iErrCode)
    {
    case TSC_ERROR_CANCELLED:
        return TSC_DBUS_INTERFACE ERR_NAME_CANCELLED;
    case TSC_ERROR_DATA_ACCESS:
        return TSC_DBUS_INTERFACE ERR_NAME_DATA_ACCESS;
    case TSC_ERROR_INVALID_PARAM:
        return TSC_DBUS_INTERFACE ERR_NAME_INVALID_PARAM;
    case TSC_ERROR_INSUFFICIENT_RES:
        return TSC_DBUS_INTERFACE ERR_NAME_INSUFFICIENT_RES;
    case TSC_ERROR_INTERNAL:
        return TSC_DBUS_INTERFACE ERR_NAME_INTERNAL;
    case TSC_ERROR_INVALID_HANDLE:
        return TSC_DBUS_INTERFACE ERR_NAME_INVALID_HANDLE;
    case TSC_ERROR_NOT_IMPLEMENTED:
        return TSC_DBUS_INTERFACE ERR_NAME_NOT_IMPLEMENTED;
    }

    /* default: TSC_ERROR_MODULE_GENERIC */
    return TSC_DBUS_INTERFACE ERR_NAME_MODULE_GENERIC;
}

const char *GetErrorDescription(const int iErrCode)
{
    switch (iErrCode)
    {
    case TSC_ERROR_CANCELLED:
        return ERR_DESC_CANCELLED;
    case TSC_ERROR_DATA_ACCESS:
        return ERR_DESC_DATA_ACCESS;
    case TSC_ERROR_INVALID_PARAM:
        return ERR_DESC_INVALID_PARAM;
    case TSC_ERROR_INSUFFICIENT_RES:
        return ERR_DESC_INSUFFICIENT_RES;
    case TSC_ERROR_INTERNAL:
        return ERR_DESC_INTERNAL;
    case TSC_ERROR_INVALID_HANDLE:
        return ERR_DESC_INVALID_HANDLE;
    case TSC_ERROR_NOT_IMPLEMENTED:
        return ERR_DESC_NOT_IMPLEMENTED;
    }

    /* default: TSC_ERROR_MODULE_GENERIC */
    return ERR_DESC_MODULE_GENERIC;
}
