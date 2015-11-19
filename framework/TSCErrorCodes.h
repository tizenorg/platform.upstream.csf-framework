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

#ifndef TSCERRORCODES_H
#define TSCERRORCODES_H

#ifdef __cplusplus 
extern "C" {
#endif
/**
 * \file TSCErrorCodes.h
 * \brief TSC Error Code Header File
 *  
 * This file provides the TSC error code definition.
 */

#define TSC_ERROR_MODULE_GENERIC 1 /* A generic error code. */
#define ERR_NAME_MODULE_GENERIC "Generic"
#define ERR_DESC_MODULE_GENERIC "A generic error."

#define TSC_ERROR_CANCELLED 1 /* Operation cancelled. */
#define ERR_NAME_CANCELLED "Cancelled"
#define ERR_DESC_CANCELLED "Operation cancelled."

#define TSC_ERROR_DATA_ACCESS 2 /* Unable to access data. */
#define ERR_NAME_DATA_ACCESS "DataAccess"
#define ERR_DESC_DATA_ACCESS "Unable to access data."

#define TSC_ERROR_INVALID_PARAM 3 /* Invalid parameter. */
#define ERR_NAME_INVALID_PARAM "InvalidParam"
#define ERR_DESC_INVALID_PARAM "Invalid parameter."

#define TSC_ERROR_INSUFFICIENT_RES 4 /* Insufficient resource. */
#define ERR_NAME_INSUFFICIENT_RES "InsufficientRes"
#define ERR_DESC_INSUFFICIENT_RES "Insufficient resource."

#define TSC_ERROR_INTERNAL 5 /* Unexpected internal error. */
#define ERR_NAME_INTERNAL "Internal"
#define ERR_DESC_INTERNAL "Unexpected internal error."

#define TSC_ERROR_INVALID_HANDLE 6 /* Invalid handle. */
#define ERR_NAME_INVALID_HANDLE "InvalidHandle"
#define ERR_DESC_INVALID_HANDLE "Invalid handle."

#define TSC_ERROR_NOT_IMPLEMENTED 7 /* Specified method is not implemented by the Channel server. */
#define ERR_NAME_NOT_IMPLEMENTED "NotImplemented"
#define ERR_DESC_NOT_IMPLEMENTED "Specified method is not implemented."

#define TSC_ERROR_REMOVE_METHOD_NOT_FOUND -1
#define TSC_ERROR_ADD_METHOD_FAILED -1

#ifdef __cplusplus
}
#endif 

#endif /* TSCERRORCODES_H */
