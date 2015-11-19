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
 * \file IpcForkDaemon.c
 * \brief Ipc Fork Daemon source file.
 *
 * This file implements the Ipc Fork Daemon function used by Security framework.
 */

#include "Debug.h"
#include "IpcForkDaemon.h"
#include <signal.h>

void
fork_daemon()
{
    pid_t pid;

    // Fork off the parent process.
    pid = fork();

    // An error occurred.
    if (pid < 0)
    {
        DERR("%s\n", "unable to start the TWPSerDaemon");
        exit(EXIT_FAILURE);
    }

    // Success: Let the parent terminate.
    if (pid > 0)
    {
        DERR("%s\n", "Successfully forked the child process");
        exit(EXIT_SUCCESS);
    }

    // On success: The child process becomes session leader.
    if (setsid() < 0)
    {
        DERR("%s\n", "unable to start the TWPSerDaemon");
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // Fork off for the second time to ensure session leading process ends.
    pid = fork();

    // An error occurred.
    if (pid < 0)
    {
        DERR("%s\n", "unable to start the TWPSerDaemon");
        exit(EXIT_FAILURE);
    }

    // Success: Let the session leading process end.
    if (pid > 0)
    {
        DERR("%s\n", "Successfully forked the TWPSerDaemon process in the new session");
        exit(EXIT_SUCCESS);
    }

    //close the file descriptors of terminal associated with the process
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
