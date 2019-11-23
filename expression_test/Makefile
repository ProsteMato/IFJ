CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
OBJ_SCANNER = scanner.o stack.o string.o
OBJ_EXPRESSION = expression_parser.o exprstack.o exprlist.o 

all: test

run:
	./test <input

test: expression_parser.o expression_parser.h exprtest.c exprstack.o exprlist.o stack.o string.o scanner.o 
	${CC} ${CFLAGS} ${OBJ_SCANNER} ${OBJ_EXPRESSION} exprtesttest.c -o $@

expression_parser.o: expression_parser.c expression_parser.h exprstack.o exprlist.o scanner.o 
	${CC} ${CFLAGS} $< -c -o $@

exprstack.o: expression_stack.c expression_stack.h 
	${CC} ${CFLAGS} $< -c -o $@

exprlist.o: expression_list.c expression_list.h
	${CC} ${CFLAGS} $< -c -o $@

scanner.o: scanner.c scanner.h stack.o string.o
	${CC} ${CFLAGS} $< -c -o $@

string.o: my_string.c my_string.h
	${CC} ${CFLAGS} $< -c -o $@

stack.o: stack.c stack.h
	${CC} ${CFLAGS} $< -c -o $@

clean:
	rm *.o test