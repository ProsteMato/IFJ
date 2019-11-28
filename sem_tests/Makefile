CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic 

all: sem-test

#run:
#	./test <input

sem-test: sematic_analysis.o expression_parser.o sem_test.c expression_stack.o expression_list.o scanner.o stack.o string.o symtable.o generator.o
	${CC} ${CFLAGS}  $^ -o ./bin/$@ -g

expression_parser.o: ../expression_parser.c ../expression_parser.h 
	${CC} ${CFLAGS} $< -c -o $@

sematic_analysis.o: ../sematic_analysis.c ../sematic_analysis.h 
	${CC} ${CFLAGS} $< -c -o $@ -g

expression_stack.o: ../expression_stack.c ../expression_stack.h 
	${CC} ${CFLAGS} $< -c -o $@

expression_list.o: ../expression_list.c ../expression_list.h
	${CC} ${CFLAGS} $< -c -o $@

symtable.o: ../symtable.c ../symtable.h 
	${CC} ${CFLAGS} $< -c -o $@ -g

generator.o: ../generator.c ../generator.h 
	${CC} ${CFLAGS} $< -c -o $@

scanner.o: ../scanner.c ../scanner.h
	${CC} ${CFLAGS} $< -c -o $@

string.o: ../my_string.c ../my_string.h
	${CC} ${CFLAGS} $< -c -o $@

stack.o: ../stack.c ../stack.h
	${CC} ${CFLAGS} $< -c -o $@

clean:
	rm *.o ./bin/sem-test