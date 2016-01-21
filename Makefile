CFLAGS=-std=c99 -pedantic -Wall -Werror

all: ipdd2hex iphex2dd hostinfo echoclient

clean:
	rm ipdd2hex iphex2dd

.PHONY: all clean

ipdd2hex: ipdd2hex.c
	$(CC) $(CFLAGS) -o $@ $^

iphex2dd: iphex2dd.c
	$(CC) $(CFLAGS) -o $@ $^

hostinfo: hostinfo.c
	$(CC) $(CFLAGS) -o $@ $^

echoclient: echoclient.c
	$(CC) $(CFLAGS) -o $@ $^
