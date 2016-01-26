#include <stdio.h>
#include <ctype.h>
#include <assert.h>

/* The maximum length of TCP hostname are 253 ASCII character; specified 
 * by RFC 952. */
#define TCP_HOSTLN 253
#define TCP_PORTLN 5

// bagaimana handle error kalo prototype begini?
// kemungkinan error adalah format salah
// atau empty host
// expected valid case : "host:port"
// expected invalid case:
// - "hostport" (invalid format)
// - ":port" (host not found)
// - "host:port:extra" (invalid format for port)
char *tcpsplithost(char hostport[]);

// return error codes. host modified from host.
// valid host itu apa?
// max length berapa? terus valid char gimana dll?
// valid hostname only contain a-zA-Z0-9 and '-'
int tcpsh(char *host, char addr[]);
int tcpsp(char *port, int colon_i, char addr[]);
int tcpsaddr(char *host, char *port, char addr[]);

enum {
    TCP_INVH = 1
};

int main()
{
    char host[TCP_HOSTLN];
    char port[TCP_PORTLN];

    char hostport1[] = "localhost:8080";
    char hostport2[] = "some.example.com:80";
    char hostport3[] = "*invalidch^r:8080";
    char hostport4[] = "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz0123456789"
                       "abcdefijklmnopqrstuvwxyz01234:56789";
    char hostport6[] = ":8080";
    char hostport5[] = "localhost8080";

    assert(tcpsaddr(host, port, hostport1) == 0);
    printf("host: %s; port: %s\n", host, port);

    assert(tcpsaddr(host, port, hostport2) == 0);
    printf("host: %s; port: %s\n", host, port);

    assert(tcpsaddr(host, port, hostport3) == -1);
    assert(tcpsaddr(host, port, hostport4) == -1);
    assert(tcpsaddr(host, port, hostport5) == -1);
    assert(tcpsaddr(host, port, hostport6) == -1);
}

/* tcpsh: splits TCP hostname from the host:port address format and write to
 * _host_. 
 * 
 * This function do not write more than TCP_HOSTLN bytes (including the
 * terminating null byte ('\0')). The lentgth of _host_ should equal to or 
 * greater than TCP_HOSTLN bytes.
 *
 * Upon successful return, this function return the index of ':'. If the 
 * _hostport_ format is invalid, -1 value is returned */
int tcpsh(char *host, char hostport[])
{
    int valid = 0;
    /* index of colon from host:port */
    int colon_i = 0;

    for(int i = 0; hostport[i] != '\0'; i++) {
        /* current character */
        char c = hostport[i];

        /* valid ASCII character are a-zA-Z0-9, '.' and '-'; as specified 
         * by RFC 952 */
        if(!(isalnum(c) || (c == '-') || (c == '.') || (c == ':'))) break;

        if(c != ':') {
            /* to make sure not to overflow the buffer _host_ */
            if(i >= (TCP_HOSTLN-1)) break;

            /* write to _host_ */
            host[i] = c;
        } else {
            /* to handle ":port" invalid format */
            if(i == 0) break;

            host[i] = '\0';
            valid = 1;
            colon_i = i;
            break;
        }
    }

    /* empty the invalid host */
    if(!valid) {
        host[0] = '\0';
        return -1;
    }

    return colon_i;
}

/* tcpsp: splits port from the host:port format address and writes to _port_.
 * It starts reading _hostport_ from _colon_i_ index until the null character.
 * 
 * This function do not write more than TCP_PORTLN bytes (including the
 * terminating null byte ('\0')). The lentgth of _port_ should equal to or 
 * greater than TCP_PORTLN bytes.
 * 
 * Upon successful return, this function returns 0. If the 
 * _hostport_ format is invalid, -1 value is returned */
int tcpsp(char *port, int colon_i, char hostport[])
{
    int valid = 1;
    int startindex = colon_i + 1;
    int port_i = 0;

    for(int i = startindex; hostport[i] != '\0'; i++) {
        /* current character */
        char c = hostport[i];

        /* validate the port */
        if(!isdigit(c)) {
            valid = 0;
            break;
        }
        if(i >= ((startindex+TCP_PORTLN)-1)) {
            valid = 0;
            break;
        }

        /* write to _port_ */
        port[port_i] = c;
        port_i++;
    }

    if(!valid) {
        port[0] = '\0';
        return -1;
    }

    return 0;
}

/* tcpsaddr: splits host and port from address _addr_ and writes to _host_
 * and _port_.
 *
 * This function combine tcpsh() and tcpsp(). The size of _host_ and _port_
 * should equal to or greater than TCP_HOSTLN and TCP_PORTLN.
 *
 * It returns 0 if success and -1 if _addr_ format is invalid.
 */
int tcpsaddr(char *host, char *port, char addr[])
{
    int colon_i = tcpsh(host, addr);
    if(colon_i == -1) return -1;
    return tcpsp(port, colon_i, addr);
}
