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
 * \file Debug.h
 * \brief TWP Service Daemon Debug header file.
 *
 * This file contains the macros used by Security framework.
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "CS_FRAMEWORK"

#ifdef DEBUG
#include <dlog/dlog.h>

#define DLOG(prio, fmt, arg...) \
    do { SLOG(prio, LOG_TAG, fmt, ##arg); } while(0);

#define DINFO(fmt, arg...) SLOGI(fmt, ##arg)
#define DDBG(fmt, arg...) SLOGD("%s:" fmt, __FUNCTION__, ##arg)
#define DWARN(fmt, arg...) SLOGW("%s:%d " fmt, __FUNCTION__, __LINE__, ##arg)
#define DERR(fmt, arg...) SLOGE("%s:%d " fmt, __FUNCTION__, __LINE__, ##arg)
#define DFATAL(fmt, arg...) \
    do { SLOG(LOG_FATAL, LOG_TAG, fmt, ##arg); } while(0);
#else
#define DLOG(prio, fmt, arg...) {}
#define DINFO(fmt, arg...) {}
#define DDBG(fmt, arg...) {}
#define DWARN(fmt, arg...) {}
#define DERR(fmt, arg...) {}
#define DFATAL(fmt, arg...) {}
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _DEBUG_H */
