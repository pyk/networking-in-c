/* tcp - A C module that provides an easy-to-use interface to work with TCP.
 *
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

/* This macro causes system header files to expose definitions corresponding 
 * to the POSIX.1-2008 base specification (excluding the XSI extension). */
#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "tcp.h"

/* tcpsh splits hostname from the address. 
 * 
 * The first parameter is a string where to write the hostname into, the size 
 * of the string should be equal or greater than TCPHOSTLN. The second 
 * parameter is an address that have the form host:port.
 *
 * If the function succeeds it returns the number of bytes that written into
 * host[].
 * If the hostname is invalid, it returns -1 and set errno to EINVAL. 
 */
static int tcpsh(char host[], char addr[])
{
    int nbytes = 0;

    for(int i = 0; addr[i] != '\0' && addr[i] != ':'; i++) {
        /* current character */
        char c = addr[i];

        /* hostname validation
         * Valid ASCII character for hostname are [a-zA-Z0-9.-] as specified 
         * by RFC 952. The maximum length of hostname are 253 ASCII characters.*/
        if(!(isalnum(c) || (c == '-') || (c == '.')) || (i >= (TCP_HOSTLN-1))) {
            goto invalid;
        }

        /* write to *host */
        host[i] = c;
        nbytes = i + 1;
    }

    if(nbytes == 0 || addr[nbytes] != ':') goto invalid;

    host[nbytes] = '\0';
    errno = 0;
    return nbytes;

invalid:
    errno = EINVAL;
    host[0] = '\0';
    return -1;
}

/* tcpsp splits port from the address. 
 * 
 * The first parameter is a string where to write the port into, the size 
 * of the string should be equal or greater than TCPPORTLN. The second 
 * parameter is an index where to start reading the address, usually the
 * valid value returned by tcpsh() is used. The third is an address that have
 * form host:port.
 *
 * If the function succeeds it returns the number of bytes that written into
 * port[].
 * If the port is invalid, it returns -1 and set errno to EINVAL. 
 */
static int tcpsp(char port[], int colon_i, char addr[])
{
    /* track the port character index */
    int port_i = 0;

    /* handle invalid "host:" format */
    if(addr[colon_i] != ':') goto invalid;

    for(int i = colon_i+1; addr[i] != '\0'; i++) {
        /* current character */
        char c = addr[i];

        /* validate the port */
        if(!isdigit(c) || port_i >= (TCP_PORTLN-1)) goto invalid;

        /* write to _port_ */
        port[port_i] = c;
        port_i++;
    }

    if(port_i == 0) goto invalid;

    port[port_i] = '\0';
    errno = 0;
    return port_i;

invalid:
    port[0] = '\0';
    errno = EINVAL;
    return -1;
}

/* tcpsaddr splits host and port from the specified address.
 *
 * The first two parameter is a string to where the host and port write in to.
 * The host and port string size should be TCP_HOSTLN bytes and TCPPORTLN 
 * bytes. The third parameter is an address that have the form host:port.
 *
 * If the function succeeds it returns 0.
 * If the hostname or port number is invalid, it returns and set errno to 
 * EINVAL.
 *
 * Example:
 *     char addr[] = "localhost:8080";
 *     char host[TCP_HOSTLN];
 *     char port[TCP_PORTLN];
 *     int err = tcpsaddr(host, port, addr);
 *     if(err != 0) {
 *         printf("error: %s\n", strerror(errno));  
 *     }
 *     printf("%s - %s", host, port); // localhost - 8080 
 */
int tcpsaddr(char *host, char *port, char addr[])
{
    int colon_i = tcpsh(host, addr);
    if(colon_i == -1) {
        errno = EINVAL;
        return EINVAL;
    }
    int res = tcpsp(port, colon_i, addr);
    if(res == -1) {
        errno = EINVAL;
        return EINVAL;
    }
    errno = 0;
    return 0;
}

/* tcpdial connects to a TCP server.
 * 
 * The first parameter is the pointer of integer where the enpoint of 
 * connection is write into. Second one is an address of TCP server to connect
 * to, it have the form host:port.
 *
 * If the function succeeds it returns 0 and the enpoint of connection can be
 * used by send(2) and recv(2).
 * If the function fails, it returns and set errno to one of the following 
 * non-zero values:
 *
 * ENOPROTOOPT
 * The TCP protocol entry is not available in the host protocols database. On the
 * linux host, the protocol database file is /etc/protocols.
 *
 * EACCES
 * The process does not have appropriate privileges.
 *
 * ENOMEM
 * Insufficient memory is available. The socket cannot be create until sufficient
 * resources are freed.
 *
 * EINVAL
 * The address is invalid.
 */
int tcpdial(int *conn, char addr[])
{
    /* get the TCP protocol number from the host; 
     * conforming to POSIX.1-2001 */
    struct protoent *tcpproto = getprotobyname("tcp");
    if(tcpproto == NULL) {
        errno = ENOPROTOOPT;
        return ENOPROTOOPT;
    }

    /* create a socket; conforming to POSIX.1-2001 */
    *conn = socket(AF_INET, SOCK_STREAM, tcpproto->p_proto);
    if(*conn == -1) {
        return errno;
    }

    /* get host and port from the address */
    char host[TCP_HOSTLN];
    char port[TCP_PORTLN];
    int retsaddr = tcpsaddr(host, port, addr);
    if(retsaddr != 0) return retsaddr;

    /* create a socket address; conforming to POSIX.1-2001 */
    struct addrinfo tcphints, *tcpsockaddr;
    memset(&tcphints, 0, sizeof tcphints);
    tcphints.ai_family = AF_INET;
    tcphints.ai_socktype = SOCK_STREAM;
    tcphints.ai_protocol = tcpproto->p_proto;
    int retgai = getaddrinfo(host, port, &tcphints, &tcpsockaddr);
    if(retgai != 0) {
        /* TODO: what should I return, there is another error number
         * outside errno.h enum */
        return TCPGAI;
    }

    /* iterate over returned address & test the connection for each address;
     * only */
    struct addrinfo *addri;
    for(addri = tcpsockaddr; addri != NULL; addri = addri->ai_next) {
        /* try connect to a socket address; conforming to POSIX.1-2001 */
        int retcon = connect(*conn, addri->ai_addr, addri->ai_addrlen);
        if(retcon != 0) {
            /* Continue until we can connect to the address */
            continue;
        }
        break;
    }
    if(addri == NULL) {
        return TCPCON;
    }

    return 0;
}
