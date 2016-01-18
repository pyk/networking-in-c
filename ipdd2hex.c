/* ipdd2hex.c - Converts IP address repesentation from decimal-dot 
 * notation to hexadecimal.
 *
 * Build:
 * % make ipdd2hex
 *
 * Usage:
 * % ./ipdd2hex 128.2.194.242
 *
 * License:
 * BSD 3-clause Revised
 * Copyright (c) 2016, Bayu Aldi Yansyah <bayualdiyansyah@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of [project] nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <decimal-dot>\n", argv[0]);
        return 1;
    }

    /* Convert input to in_addr first */
    struct in_addr addr;
    int ptonret = inet_pton(AF_INET, argv[1], &addr);
    if(ptonret == 0) {
        fprintf(stderr, "Invalid IP address\n");
        return 1;
    }
    if(ptonret == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    }

    /* print the hexadecimal representation in network byte order */
    printf("%#x\n", htonl(addr.s_addr));
    return 0;
}
