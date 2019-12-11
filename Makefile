##
# FILE: Makefile
# breaf: Makefile will create program for translation of IFJ19

CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic -g
OBJ = scanner.o symtable.o stack.o main.o parser.o generator.o scanner.o my_string.o sematic_analysis.o expression_parser.o expression_list.o expression_stack.o

all: cifj19

cifj19: ${OBJ}
	${CC} ${CFLAGS} $^ -o $@

clean:
	rm *.o cifj19

test: 
	bash ./run_tests.sh

# --------------------------#
#         Object files      #
# --------------------------#
main.o: main.c main.h
	${CC} ${CFLAGS} -c $< -o $@

scanner.o: scanner.c scanner.h
	${CC} ${CFLAGS} -c $< -o $@

symtable.o: symtable.c symtable.h
	${CC} ${CFLAGS} -c $< -o $@

parser.o: parser.c parser.h
	${CC} ${CFLAGS} -c $< -o $@

generator.o: generator.c generator.h
	${CC} ${CFLAGS} -c $< -o $@

my_string.o: my_string.c my_string.h
	${CC} ${CFLAGS} -c $< -o $@

sematic_analysis.o: sematic_analysis.c sematic_analysis.h
	${CC} ${CFLAGS} -c $< -o $@

expression_parser.o: expression_parser.c expression_parser.h
	${CC} ${CFLAGS} -c $< -o $@

stack.o: stack.c stack.h
	${CC} ${CFLAGS} -c $< -o $@

expression_stack.o: expression_stack.c expression_stack.h
	${CC} ${CFLAGS} -c $< -o $@

expression_list.o: expression_list.c expression_list.h
	${CC} ${CFLAGS} -c $< -o $@