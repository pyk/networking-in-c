/* echoclient-module
 * This is different version of echoclient.c. All functionality of TCP is
 * extracted to tcp.o module.
 *
 * Build:
 * % make tcp.o
 * % make echoclient-module
 *
 * Usage:
 * % ./echoclient-module localhost 8080 hello
 *
 * License:
 * BSD 3-clause Revised
 * Copyright (c) 2016, Bayu Aldi Yansyah <bayualdiyansyah@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of its 
 *        contributors may be used to endorse or promote products derived
 *        from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
 
#include "tcp.h"

int main(int argc, char **argv)
{
    if(argc != 4) {
        fprintf(stderr, "Usage: %s host port message\n", argv[0]);
        return 1;
    }

    int conn;
    errno = tcpdial(&conn, argv[1], argv[2]);
    if(errno != 0) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return 1;
    }

    /* send message to a socket */
    int retsend = send(conn, argv[3], strlen(argv[3]), MSG_DONTWAIT);
    if(retsend == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return 1;
    }

    /* read a message from socket */
    char buf[100];
    int retrecv = recv(conn, buf, 100, MSG_DONTWAIT);
    if(retrecv == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return 1;
    }

    printf("message: %s\n", buf);
    return 0;
}
