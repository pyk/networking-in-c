/* This macro causes header files to expose definitions corresponding to the
 * POSIX.1-2008 base specification (excluding the XSI extension). */
#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>

#include "tcp.h"

/* tcpsh: splits TCP hostname from the hostport[] address  and write to *host.
 * This function will start reading the hostport[] from the first index until 
 * the ':' character found. 
 * 
 * Valid ASCII character for hostname are [a-zA-Z0-9.-] as specified 
 * by RFC 952. The maximum length of hostname are 253 ASCII characters.
 *
 * This function do not write more than TCP_HOSTLN bytes (including the
 * terminating null byte ('\0')) to *host. The size of *host should equal 
 * to or greater than TCP_HOSTLN bytes.
 *
 * Upon successful return, this function return the number or bytes that 
 * succesfully written to *host. If the hostname is invalid or ':' character
 * is not found in hostport[], -1 will be returned. */
int tcpsh(char *host, char hostport[])
{
    int nbytes = 0;

    for(int i = 0; hostport[i] != '\0' && hostport[i] != ':'; i++) {
        /* current character */
        char c = hostport[i];

        /* hostname validation */
        if(!(isalnum(c) || (c == '-') || (c == '.')) || (i >= (TCP_HOSTLN-1))) {
            host[0] = '\0';
            return -1;
        };

        /* write to *host */
        host[i] = c;
        nbytes = i + 1;
    }

    if(nbytes == 0 || hostport[nbytes] != ':') {
        host[0] = '\0';
        return -1;
    }

    host[nbytes] = '\0';
    return nbytes;
}

/* tcpsp: splits port from the hostport[] address and write to *port. This
 * function will start reading the hostport[] from colon_i index until the null
 * ('\0') character.
 * 
 * Since the port is stored in 16-bit integer, valid port are only a 5 digit 
 * ASCII characters long.
 * 
 * This function do not write more than TCP_PORTLN bytes (including the
 * terminating null byte ('\0')) to *port. The length of *port should 
 * equal to or greater than TCP_PORTLN bytes.
 * 
 * Upon successful return, this function returns the number of bytes that
 * succesfully written to *port. If the port is invalid, -1 is returned */
int tcpsp(char *port, int colon_i, char hostport[])
{
    int port_i = 0;

    /* handle invalid "host:" format */
    if(hostport[colon_i] != ':') return -1;

    for(int i = colon_i+1; hostport[i] != '\0'; i++) {
        /* current character */
        char c = hostport[i];

        /* validate the port */
        if(!isdigit(c) || port_i >= (TCP_PORTLN-1)) {
            port[0] = '\0';
            return -1;
        }

        /* write to _port_ */
        port[port_i] = c;
        port_i++;
    }

    if(port_i == 0) {
        port[0] = '\0';
        return -1;
    }

    port[port_i] = '\0';
    return port_i;
}

/* tcpsaddr: splits host and port from the address addr[] and write to *host
 * and *port.
 *
 * This function combine tcpsh(3) and tcpsp(3) functions. The size of *host 
 * and *port should equal to or greater than TCP_HOSTLN bytes and 
 * TCP_PORTLN bytes.
 *
 * This function returns 0 if successfully split host and port or TCPINVH if
 * the hostname in invalid and TCPINVP if the port is invalid.
 */
int tcpsaddr(char *host, char *port, char addr[])
{
    int colon_i = tcpsh(host, addr);
    if(colon_i == -1) return TCPINVH;
    int res = tcpsp(port, colon_i, addr);
    if(res == -1) return TCPINVP;
    return 0;
}

/* tcpdial: connects to the address addr[] on the TCP network and write socket
 * file descriptor to *sockfd. The valid address format is "host:port".
 *
 * Upon successful return, this function return 0 and *sockfd is can be used
 * for send(2) and recv(2). If any error happen, non-zero value will be 
 * returned. */
int tcpdial(int *sockfd, char addr[])
{
    /* get the TCP protocol number from the host; 
     * conforming to POSIX.1-2001 */
    struct protoent *tcpproto = getprotobyname("tcp");
    if(tcpproto == NULL) {
        return TCPNF;
    }

    /* create a socket; conforming to POSIX.1-2001 */
    *sockfd = socket(AF_INET, SOCK_STREAM, tcpproto->p_proto);
    if(*sockfd == -1) {
        return TCPSC;
    }

    char host[TCP_HOSTLN];
    char port[TCP_PORTLN];
    int retsaddr = tcpsaddr(host, port, addr);
    if(retsaddr != 0) return retsaddr;

    /* create a socket address */
    struct addrinfo tcphints, *tcpsockaddr;
    memset(&tcphints, 0, sizeof tcphints);
    tcphints.ai_family = AF_INET;
    tcphints.ai_socktype = SOCK_STREAM;
    tcphints.ai_protocol = tcpproto->p_proto;
    int retgai = getaddrinfo(host, port, &tcphints, &tcpsockaddr);
    if(retgai != 0) {
        return TCPGAI;
    }

    /* iterate over returned address & perform connect */
    struct addrinfo *addri;
    for(addri = tcpsockaddr; addri != NULL; addri = addri->ai_next) {
        /* connect to a socket addriess */
        int retcon = connect(*sockfd, addri->ai_addr, addri->ai_addrlen);
        if(retcon != 0) {
            continue;
        }
        break;
    }
    if(addri == NULL) {
        return TCPCON;
    }

    return 0;
}
