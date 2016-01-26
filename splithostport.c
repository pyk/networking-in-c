#include <stdio.h>
#include <ctype.h>
#include <assert.h>

/* The maximum length of TCP hostname are 253 ASCII character; specified 
 * by RFC 952. */
#define TCP_HOSTLN 254
/* TCP port is stored in 16-bit integer, the maximum value is 65535; 5
 * digit characters long. */
#define TCP_PORTLN 6

int tcpsh(char *host, char addr[]);
int tcpsp(char *port, int colon_i, char addr[]);
int tcpsaddr(char *host, char *port, char addr[]);

enum {
    TCP_INVH = 1,
    TCP_INVP
};

int main()
{
    char host[TCP_HOSTLN];
    char port[TCP_PORTLN];

    /* valid example */
    char addr1[] = "localhost:8080";
    char addr2[] = "some.example.com:80";

    assert(tcpsaddr(host, port, addr1) == 0);
    printf("host: %s; port: %s\n", host, port);
    assert(tcpsaddr(host, port, addr2) == 0);
    printf("host: %s; port: %s\n", host, port);

    /* invalid host */
    char addr3[] = "*invalidch^r:8080";
    char addr4[] = "invalid8080";
    char addr5[] = ":8080";
    char addr6[] = "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz01234:5678";

    assert(tcpsaddr(host, port, addr3) == TCP_INVH);
    assert(tcpsaddr(host, port, addr4) == TCP_INVH);
    assert(tcpsaddr(host, port, addr5) == TCP_INVH);
    assert(tcpsaddr(host, port, addr6) == TCP_INVH);


    /* invalid port */
    char addr7[] = "host:abc";
    char addr8[] = "host:123456";
    char addr9[] = "host:";
    char addr10[] = "host:8080:extra";

    assert(tcpsaddr(host, port, addr7) == TCP_INVP);
    assert(tcpsaddr(host, port, addr8) == TCP_INVP);
    assert(tcpsaddr(host, port, addr9) == TCP_INVP);
    assert(tcpsaddr(host, port, addr10) == TCP_INVP);

}

/* tcpsh: splits TCP hostname from the host:port address format and write to
 * *host. This function will start reading the hostport[] from the first index 
 * until the ':' character.
 * 
 * Valid ASCII character for hostname are a-zA-Z0-9 and {'.','-'} as specified 
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

/* tcpsp: splits port from the host:port format address and write to *port.
 * This function will start reading the hostport[] from colon_i index until
 * the null character.
 * 
 * Since port is stored in 16-bit interger, valid port are only a 5 digit 
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

/* tcpsaddr: splits host and port from address addr[] and write to *host
 * and *port.
 *
 * This function combine tcpsh() and tcpsp() functions. The size of *host 
 * and *port should equal to or greater than TCP_HOSTLN and TCP_PORTLN.
 *
 * It returns 0 if successfully split host and port. 
 * It returns TCP_INVH if the host is invalid.
 * It returns TCP_INVP if the port is invalid.
 */
int tcpsaddr(char *host, char *port, char addr[])
{
    int colon_i = tcpsh(host, addr);
    if(colon_i == -1) return TCP_INVH;
    int res = tcpsp(port, colon_i, addr);
    if(res == -1) return TCP_INVP;
    return 0;
}
