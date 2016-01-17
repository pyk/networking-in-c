/* ipdd2hex.c - converts IP address decimal dot notation to 
 * hexadecimal representation */
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if(argc < 2 || argc > 2) {
        fprintf(stderr, "Usage: %s <decimal-dot>\n", argv[0]);
        return 1;
    }

    struct in_addr addr;
    if(inet_aton(argv[1], &addr) == 0) {
        fprintf(stderr, "Invalid address\n");
        return 1;
    }

    printf("%#x\n", addr.s_addr);
    return 0;
}