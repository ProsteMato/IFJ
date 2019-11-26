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
#include "error.h"

#define HEADER_SIZE 11
#define ADD_INST0 (inst) CL_add_inst(inst, NULL, NULL, NULL)
#define ADD_INST1 (inst,p1) CL_add_inst(inst, p1, NULL, NULL)
#define ADD_INST2 (inst,p1,p2) CL_add_inst(inst, p1, p2, NULL)
#define ADD_INST3 (inst,p1,p2,p3) CL_add_inst(inst, p1, p2, p3)

// linked list of code for printing at the end
typedef struct{
	char *inst;
	char *adr0;
	char *adr1;
	char *adr2;
	Code_line *next;
} Code_line;

typedef struct{
	Code_line *first;
	Code_line *last;
} Code_list;

Code_list code;

void CL_init();
int CL_add_inst(char *inst,	char *adr0,	char *adr1,	char *adr2);
void CL_destroy(Code_line *line);

int init_generator();
void print_final_code();

// funkcie na generovanie IFJcode19

void gen_header(); // generovanie zac programu

// vstavane fukcie
void gen_def();
void gen_if();
void gen_else();
void gen_pass();
void gen_while();
void gen_inputs();
void gen_inputi();
void gen_inputf();
void gen_print();
void gen_len();
void gen_substr();
void gen_ord();
void gen_chr();

// operacie mat
void gen_add();
void gen_minus();
void gen_mult();
void gen_div();
void gen_idiv();
void gen_concat();

// while
void gen_while_begin(); // while start
void gen_while_end(); // while end


// funkcie

// generovanie jumpu
// hlavicka funkcie
// ukoncenie funkcie
// call funckcie



// stack na parametre
// unique labely
// int2float
// // lokalnu alebo globalnu tabulku symbolov
#endif //_GENERATOR_H