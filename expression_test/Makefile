CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic 
all: test

#run:
#	./test <input

test:  expression_parser.o ../expression_parser.h expr_test.c expression_stack.o expression_list.o scanner.o stack.o string.o 
	${CC} ${CFLAGS}  $^ -o $@

expression_parser.o: ../expression_parser.c ../expression_parser.h ../scanner.h
	${CC} ${CFLAGS} $< -c -o $@

expression_stack.o: ../expression_stack.c ../expression_stack.h 
	${CC} ${CFLAGS} $< -c -o $@

expression_list.o: ../expression_list.c ../expression_list.h ../expression_parser.h
	${CC} ${CFLAGS} $< -c -o $@

scanner.o: ../scanner.c ../scanner.h
	${CC} ${CFLAGS} $< -c -o $@

string.o: ../my_string.c ../my_string.h
	${CC} ${CFLAGS} $< -c -o $@

stack.o: ../stack.c ../stack.h
	${CC} ${CFLAGS} $< -c -o $@

clean:
	rm *.o test