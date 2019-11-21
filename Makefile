CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
OBJ_SCANNER = scanner.o

all: scanner

scanner: scanner.o scanner.h
	${CC} ${CFLAGS} ${OBJ_SCANNER} -o $@

test-parser: ./Tests/parserTest.c parser.o scanner.o
	${CC} ${CFLAGS} $^ -o $@

parser: parser.o scanner.o parser.h
	${CC} ${CFLAGS} $^ -o $@

scanner.o: scanner.c scanner.h
	${CC} ${CFLAGS} $< -o $@

parser.o: parser.c scanner.o parser.h
	${CC} ${CFLAGS} $< -o $@
clean:
	rm *.o scanner