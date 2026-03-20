CC     ?= cc
CFLAGS ?= -Wall -Wextra -std=c99 -O2

CC65_TARGET = cx16
CC65        = cl65
CC65FLAGS   = --cpu 65c02 -t $(CC65_TARGET) --create-dep $(<:.c=.d) -Ors
OBJDIR      = .obj

all: tests cli

# --- native static library ---
lib: libinventory.a

libinventory.a: inventory.o
	ar rcs $@ $^

# --- x16 library object (consumed by root makelib) ---
lib-x16: $(OBJDIR)/inventory.o

$(OBJDIR)/inventory.o: inventory.c
	@mkdir -p $(OBJDIR)
	$(CC65) -c $(CC65FLAGS) -I../include -o $@ $<

# --- native test / cli builds ---
tests: tests.o inventory.o
	$(CC) $(CFLAGS) -o tests tests.o inventory.o

cli: cli.o inventory.o
	$(CC) $(CFLAGS) -o cli cli.o inventory.o

%.o: %.c
	$(CC) $(CFLAGS) -I../include -c $< -o $@

clean:
	rm -f *.o tests cli libinventory.a inventory.d
	rm -f $(OBJDIR)/inventory.o

.PHONY: all lib lib-x16 clean
