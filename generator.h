/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief IFJcode19 generator header file
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "scanner.h"
#include "error.h"
#include "my_string.h"

#define INT2STR_SIZE 12 // vsetky cisla velkosti int sa dokazu zmestit do 12 znakov
int while_c = 0;

// linked list of code for printing at the end
typedef struct{
	char *inst;
	size_t cap;
	size_t len;
} Code;

typedef struct Code_line Code_line;
struct Code_line{
	Code *code;
	Code_line *next;
};

typedef struct{
	Code_line *first;
	Code_line *last;
} Code_list;

Code_list code_list;

// vytvorit tabulku s premennymi
char* int_to_str(int i);
char* get_var_adr(char *var);

void CL_init();
int CL_add_inst(char *inst,	char *adr0,	char *adr1,	char *adr2);
void CL_destroy(Code_line *line);

int init_generator();
void print_final_code();

char* get_var_adr(char *dest); // vymysliet
char* create_code(char *code);

// funkcie na generovanie IFJcode19

int gen_header(); // generovanie zac programu
int gen_int2float(char *var);
int gen_assing_const_to_val(char *var, char *const, Token type); // type - TK_ int/float/str


// vstavane fukcie
int gen_inputs(char *dest);
int gen_inputi(char *dest);
int gen_inputf(char *dest);
int gen_print(char *symb);
int gen_len();
int gen_substr();
int gen_ord();
int gen_chr();

// podmienky
//int gen_if();
//int gen_else();

// operacie mat
//int gen_add();
//int gen_minus();
//int gen_mult();
//int gen_div();
//int gen_idiv();
//int gen_concat();

// operacie relacne
int gen_less_than(char *op1, char *op2); //is_variable_defined
int gen_more_than(char *op1, char *op2);
int gen_equal(char *op1, char *op2);
int gen_equal_less(char *op1, char *op2);
int gen_equal_more(char *op1, char *op2);
int gen_not_equal(char *op1, char *op2);

// while
int gen_while_label();
int gen_while_begin(); // condition
int gen_while_end();


// funkcie
int gen_f_start(char *id); // pri def
int gen_f_end(char *id); // na koniec funkcie, za poslednym vygenerovanym prikazom tela f
int gen_f_call(char *id);
int gen_f_prep(); // pop zo stacku


// stack na parametre
// unique labely
// // lokalnu alebo globalnu tabulku symbolov
// na konci prejst zoznam a prehladat pouzitie built in funcii, tlacit len pouzite
// prechod zase kvoli
#endif //_GENERATOR_H