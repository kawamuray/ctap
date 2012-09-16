# Makefile for ctap

CC         = gcc
CFLAGS     =
LIBS       = -lm
SRCDIR     = src

all: $(SRCDIR)/ctap.o

$(SRCDIR)/ctap.o: $(SRCDIR)/newctap.h

clean:
	-rm -rf $(SRCDIR)/ctap.o

test:
	$(CC) misc/main.c -o misc/test.out
	misc/test.out

.c.o:
	$(CC) $(CFLAGS) $(LIBS) -g -c $< -o $@
