
PRJ=symtable
#
PROGS=$(PRJ)-test
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm

all: $(PROGS)

$(PRJ)-test: ../$(PRJ).c $(PRJ)-test.c
	$(CC) $(CFLAGS) -o $@ ../$(PRJ).c $(PRJ)-test.c -g

clean:
	rm -f *.o *.out $(PROGS)
#
