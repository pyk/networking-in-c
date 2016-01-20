/* iphex2dd.c - Converts IP address repesentation from hexadecimal 
 * to decimal-dot notation.
 *
 * Build:
 * % make iphex2dd
 *
 * Usage:
 * % ./iphex2dd 0x8002c2f2
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <hexadecimal>\n", argv[0]);
        return 1;
    }

    /* Convert hexadecimal to integer */
    int input;
    if(sscanf(argv[1], "%x", &input) == EOF) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return 1;
    }

    /* Set the input as the value of addr.s_addr */
    struct in_addr addr;
    addr.s_addr = htonl(input);

    /* get the decimal dot notation string */
    char ddstr[INET_ADDRSTRLEN]; 
    inet_ntop(AF_INET, &addr, ddstr, INET_ADDRSTRLEN);
    printf("%s\n", ddstr);
    
    return 0;
}
