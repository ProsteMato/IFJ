CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
OBJ_SCANNER = scanner.o

all: test-parser

test-parser: parserTest.c parser.o scanner.o stack.o my_string.o generator.o symtable.o sematic_analysis.o
	${CC} ${CFLAGS} $^ -o ./bin/$@ -g

stack.o: ../stack.c ../stack.h
	${CC} -c ${CFLAGS} $< -o $@

generator.o: ../generator.c ../generator.h
	${CC} -c ${CFLAGS} $< -o $@

sematic_analysis.o: ../sematic_analysis.c ../sematic_analysis.h
	${CC} -c ${CFLAGS} $< -o $@

symtable.o: ../symtable.c ../symtable.h
	${CC} -c ${CFLAGS} $< -o $@

my_string.o: ../my_string.c ../my_string.h
	${CC} -c ${CFLAGS} $< -o $@

scanner.o: ../scanner.c ../scanner.h
	${CC} -c ${CFLAGS} $< -o $@

parser.o: ../parser.c  ../parser.h
	${CC} -c ${CFLAGS} $< -o $@

clean:
	rm *.o ./bin/test-parser