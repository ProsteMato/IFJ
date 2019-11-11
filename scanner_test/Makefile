CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
OBJ_SCANNER = scanner.o stack.o string.o

all: test

run:
	./test < input

test: scanner.o scanner.h scantest.c stack.o string.o
	${CC} ${CFLAGS} ${OBJ_SCANNER} scantest.c -o $@

scanner.o: scanner.c scanner.h stack.o string.o
	${CC} ${CFLAGS} $< -c -o $@

string.o: my_string.c my_string.h
	${CC} ${CFLAGS} $< -c -o $@

stack.o: stack.c stack.h
	${CC} ${CFLAGS} $< -c -o $@

clean:
	rm *.o test