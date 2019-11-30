CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
OBJ_SCANNER = scanner.o

all: test-parser

test-parser: parserTest.c parser.o scanner.o stack.o my_string.o generator.o symtable.o sematic_analysis.o expretion_stack.o expretion_parser.o expretion_list.o
	${CC} ${CFLAGS} $^ -o ./bin/$@ -g

stack.o: ../stack.c ../stack.h
	${CC} -c ${CFLAGS} $< -o $@ -g 

expretion_parser.o: ../expression_parser.c ../expression_parser.h
	${CC} -c ${CFLAGS} $< -o $@ -g 

expretion_list.o: ../expression_list.c ../expression_list.h
	${CC} -c ${CFLAGS} $< -o $@ -g 

expretion_stack.o: ../expression_stack.c ../expression_stack.h
	${CC} -c ${CFLAGS} $< -o $@ -g 

generator.o: ../generator.c ../generator.h
	${CC} -c ${CFLAGS} $< -o $@ -g 

sematic_analysis.o: ../sematic_analysis.c ../sematic_analysis.h
	${CC} -c ${CFLAGS} $< -o $@ -g

symtable.o: ../symtable.c ../symtable.h
	${CC} -c ${CFLAGS} $< -o $@ -g

my_string.o: ../my_string.c ../my_string.h
	${CC} -c ${CFLAGS} $< -o $@ -g

scanner.o: ../scanner.c ../scanner.h
	${CC} -c ${CFLAGS} $< -o $@ -g

parser.o: ../parser.c  ../parser.h
	${CC} -c ${CFLAGS} $< -o $@ -g

clean:
	rm *.o ./bin/test-parser