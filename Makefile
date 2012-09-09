# Makefile for ctap

CC         = gcc
CFLAGS     =
LIBS       = -lm
SRCDIR     = src

all: $(SRCDIR)/ctap.o

$(SRCDIR)/ctap.o: $(SRCDIR)/ctap.c $(SRCDIR)/ctap.h

clean:
	-rm -rf $(SRCDIR)/ctap.[cho]

test:
	$(CC) misc/main.c misc/ctap.c -o misc/test.out
	misc/test.out

.c.o:
	$(CC) $(CFLAGS) $(LIBS) -g -c $< -o $@
