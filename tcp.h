#ifndef TCP_H
#define TCP_H

/* The maximum length of TCP hostname are 253 ASCII character; specified 
 * by RFC 952. */
#define TCP_HOSTLN 254

/* TCP port is stored in 16-bit integer, the maximum value is 65535; 5
 * digit characters long. */
#define TCP_PORTLN 6

/* List of return values if error happen */
enum {
    /* Invalid TCP host address */
    TCPINVH = 1,
    /* Invalid TCP port */
    TCPINVP,
    /* TCP protocol not found in host protocol database */
    TCPNF,
    /* Error from socket(3) syscall; error message is available
     * via errno */
    TCPSC,
    /* Error from getaddrinfo(3) syscall; error message is available
     * via errno */
    TCPGAI,
    /* Cannot connect to addr */
    TCPCON
};

int tcpsh(char *host, char addr[]);
int tcpsp(char *port, int colon_i, char addr[]);
int tcpsaddr(char *host, char *port, char addr[]);
int tcpdial(int *sockfd, char addr[]);

#endif
