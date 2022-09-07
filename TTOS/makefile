CC=gcc
CFLAGS=-g -Wall -Iinclude
LFLAGS=

OBJS=$(patsubst %.c,%.o,$(wildcard src/*.c)) $(patsubst %.c,%.o,$(wildcard src/fs/*.c))

all: ttos

ttos: $(OBJS)
	$(CC) $(LFLAGS) -o ttos $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) ttos