CC ?= cc
CFLAGS ?= -Wall -Wextra -std=c99 -O2

all: tests cli

tests: tests.o inventory.o
	$(CC) $(CFLAGS) -o tests tests.o inventory.o

cli: cli.o inventory.o
	$(CC) $(CFLAGS) -o cli cli.o inventory.o

%.o: %.c
	$(CC) $(CFLAGS) -I../include -c $< -o $@

clean:
	rm -f *.o tests cli
