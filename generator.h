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
#include "symtable.h"
#include "sematic_analysis.h"
#include "main.h"

#define INT2STR_SIZE 12 // vsetky cisla velkosti int sa dokazu zmestit do 12 znakov
#define FLOAT2STR_SIZE 23 // mas rozsah floatu prebehnutym %a formatom
int while_counter;
int if_counter;

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
char* int_to_str(int i); //
char* float_to_str(char *f); //
char* transform_for_write(char *str); //

void CL_init(); //
void CL_destroy(Code_line *line); // 
int CL_add_line(Code *line); //

int init_generator(); //
void print_final_code(); //

Code* create_code(); //

// funkcie na generovanie IFJcode19

int gen_header(); // generovanie zac programu //
int gen_int2float(char *var); // netestovane
int gen_assing_const_to_val(char *var, Token *token); // type - TK_ int/float/str


// vstavane fukcie
int gen_inputs(char *dest); // netestovane
int gen_inputi(char *dest); // netestovane
int gen_inputf(char *dest); // netestovane
int gen_print(char *symb); // netestovane
int gen_len(); //
int gen_substr(); //
int gen_ord(); //
int gen_chr(); //

// podmienky
int gen_if();
int gen_else();
int gen_if_end();

// operacie mat
int gen_expr();
int gen_stack_plus();
int gen_stack_mult();
int gen_stack_div();
int gen_stack_idiv();
int gen_stack_equal();
int gen_stack_notequal();
int gen_stack_less();
int gen_stack_lesseq();
int gen_stack_greater();
int gen_stack_greatereq();
int assign_expr_rest(char *dest);
int gen_add(char *op1, char *op2);
int gen_minus(char *op1, char *op2);
int gen_mult(char *op1, char *op2);
int gen_div(char *op1, char *op2);
int gen_idiv(char *op1, char *op2);
int gen_concat(char *op1, char *op2);

// while
int gen_while_label(); //
int gen_while_begin(); // condition //
int gen_while_end(); //


// funkcie
int gen_f_start(char *id); // pri def //
int gen_f_end(char *id); // na koniec funkcie, za poslednym vygenerovanym prikazom tela f //
int gen_f_call(char *id); //
int gen_f_prep_params();

// unique labely
// na konci prejst zoznam a prehladat pouzitie built in funcii, tlacit len pouzite
// prechod zase kvoli
#endif //_GENERATOR_H