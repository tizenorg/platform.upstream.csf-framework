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

#ifndef TPCS_SER_DAEMON_TESTUTILS_H
#define TPCS_SER_DAEMON_TESTUTILS_H
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>


int TerminateProcess(pid_t pid);
void TerminateAllProcess(const char *szName);
pid_t StartTPCSSerDaemon(void);
pid_t AddPlugApp(void);

void CleanupReply(char ***pArr, int *pLen);
char *GetNextUnsupportedMethodN(char **req_argv);
void ConSendMessage(TestCase *pCtx, IpcClientInfo *pInfo, ConTestContext conCtxs[],
                    pthread_t threads[], int lenCtxs, MethodCall methods[], int timeout);
void InThreadSendMessageAsync(TestCase *pCtx, IpcClientInfo *pInfo,
                              ConTestContext conCtxs[], int lenCtxs,
                              MethodCall methods[], int timeout);
void InThreadSendMessageCancelAsync(TestCase *pCtx, IpcClientInfo *pInfo,
                                    ConTestContext conCtxs[], int lenCtxs,
                                    MethodCall methods[], int timeout);


#endif  /* TPCS_SER_DAEMON_TESTUTILS_H */