CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
OBJ_SCANNER = scanner.o

all: scanner

scanner: scanner.o scanner.h
	${CC} ${CFLAGS} ${OBJ_SCANNER} -o $@

scanner.o: scanner.c scanner.h
	${CC} ${CFLAGS} $< -o $@

clean:
	rm *.o scanner