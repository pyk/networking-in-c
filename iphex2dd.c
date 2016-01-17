/* iphex2dd.c - converts IP Address from hexadecimal representation
 * to decimal dot notation */
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if(argc < 2 || argc > 2) {
        fprintf(stderr, "Usage: %s <hexadecimal>\n", argv[0]);
        return 1;
    }

    struct in_addr addr;
    if(inet_aton(argv[1], &addr) == 0) {
        fprintf(stderr, "Invalid address\n");
        return 1;
    }

    printf("%s\n", inet_ntoa(addr));
    return 0;
}