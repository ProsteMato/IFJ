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
#define FLOAT2STR_SIZE 23 // nas rozsah floatu prebehnutym %a formatom

int while_counter;
Stack while_stack;
Stack if_stack;
int if_counter;
TKQueue pq;


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
Code_list builtin_list;

//----------------------------------------
//     INTERNAL TRANSFORM FUNCTIONS      -
//----------------------------------------

/**
 * @brief      Makes string containing integer i
 *
 * @param[in]  i     Integer
 *
 * @return     NULL of pointer to the result string
 */
char* int_to_str(int i);
/**
 * @brief      Transforms the float in string to the %a format
 *
 * @param      f     Pointer to the string containing float
 *
 * @return     NULL of pointer to the result string
 */
char* float_to_str(char *f);
/**
 * @brief      Transforms to the string to the format compatible with WRITE instruction
 *
 * @param      str   The string
 *
 * @return     NULL of pointer to the result string
 */
char* transform_for_write(char *str);

//----------------------------------------
//     INTERNAL GENERATOR FUNCTIONS      -
//----------------------------------------

/**
 * @brief      Initiates code list
 */
void CL_init();
// @brief      Destroys and deallocates the memore of code list
//
// @param      line  Pointer to the first line of code list
//
void CL_destroy(Code_line *line);
/**
 * @brief      Adds line to the end of code list
 *
 * @param      line  Pointer to the line 
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int CL_add_line(Code_list *code_list, Code *line);
/**
 * @brief      Initializes the generator.
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int init_generator();
/**
 * @brief      Prints the final code.
 */
void print_final_code();
/**
 * @brief      Creates new code structure containing the instruction and meta
 *             data to work with dynamic string which is intruction
 *
 * @return     Pointer to the newly allocated Code struct
 */
Code* create_code();

/**
 * @brief      Adds string to the code
 *
 * @param      code  Pointer to the code
 * @param      inst  String with instruction
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int add_code(Code *code, char *inst);

//----------------------------------------
//         PARAM QUEUE FUNCTIONS         -
//----------------------------------------

/**
 * @brief      { function_description }
 */
void pq_init();
/**
 * @brief      { function_description }
 *
 * @param      token    The token
 * @param[in]  ret_val  The ret value
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int pq_queue(Token *token, int ret_val);
/**
 * @brief      { function_description }
 *
 * @param      token  The token
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int pq_dequeue(Token* token);
/**
 * @brief      { function_description }
 *
 * @param      token  The token
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int pq_first(Token *token);
/**
 * @brief      Deallocates the memory of param queue
 */
void pq_destroy();

//----------------------------------------
//     FUNCTIONS FOR CODE GENERATION     -
//----------------------------------------

/**
 * @brief      Generates return inside the function, move value from top of the stack to the retval
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_return();
/**
 * @brief      Generates the header of IFJcode19 file
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_header();
/**
 * @brief      Generates code to convert integer to float
 *
 * @param      var   Pointer to the string with name of variable which holds integer
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_int2float(char *var);
/**
 * @brief      Generates code to create new var and assign the const value to it
 *
 * @param      var    Pointer to the string with name of variable
 * @param      token  Pointer to the token with value that needs to be assingned
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_assing_const_to_val(char *var, Token *token);
/**
 * @brief      Generates definition of new variable
 *
 * @param      var   Pointer to the string with name of variable
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_defvar(char *var);

//----------------------------------------
//          BUILT-IN FUNCTIONS           -
//----------------------------------------

/**
 * @brief      Generates built-in function inputs
 *
 * @param      dest  Pointer to the string with name of destination variable
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_inputs(char *dest);
/**
 * @brief      Generates built-in function inputi
 *
 * @param      dest  Pointer to the string with name of destination variable
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_inputi(char *dest);
/**
 * @brief      Generates built-in function intputf
 *
 * @param      dest  Pointer to the string with name of destination variable
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_inputf(char *dest);
/**
 * @brief      Generates built-in function print
 *
 * @param      symb  The pointer to the string with name of var or const string
 *                   to be pritent
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_print(char *symb);
/**
 * @brief      Generates built-in function len
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_len();
/**
 * @brief      Generates built-in function substr
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_substr();
/**
 * @brief      Generates built-in function ord
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_ord();
/**
 * @brief      Generates built-in function chr
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_chr();

//----------------------------------------
//             CONDITIONS                -
//----------------------------------------

/**
 * @brief      Generates the conditional jump for if statetment
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_if();
/**
 * @brief      Generates the else part of codiotions, JUMP to the end, and LABEL
 *             false for when condition isnt true
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_else();
/**
 * @brief      Generates the LABEL for end if conditional jump
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_if_end();

/**
 * @brief      Generates the code to check to type and values of expression used in if
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_if_exprval_check();

//----------------------------------------
//           MATH OPERATIONS             -
//----------------------------------------

/**
 * @brief      Generates the expression proccessing using the operand and
 *             precedence rules
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_expr();
/**
 * @brief      Generates function for addition operation proccessing using stack
 *             operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_plus();
/**
 * @brief      Generates function for substraction operation proccessing using
 *             stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_minus();
/**
 * @brief      Generates function for multiplacation operation proccessing using
 *             stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_mult();
/**
 * @brief      Generates function for division operation proccessing using stack
 *             operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_div();
/**
 * @brief      Generates function for integer division operation proccessing
 *             using stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_idiv();
/**
 * @brief      Generates function for equal (==) operation proccessing using
 *             stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_equal();
/**
 * @brief      Generates function for bot equal (!=) operation proccessing using
 *             stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_notequal();
/**
 * @brief      Generates function for lesser (<) operation proccessing using
 *             stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_less();
/**
 * @brief      Generates function for lesser of equal (=<) operation proccessing
 *             using stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_lesseq();
/**
 * @brief      Generates function for greater (>) operation proccessing using
 *             stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_greater();
/**
 * @brief      Generates function for greater or equal (=>) operation
 *             proccessing using stack operations
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_stack_greatereq();
/**
 * @brief      Stores the result of expression temporarily saved on stack to the dest var
 *
 * @param      dest  Pointer to the string with name of destination variable
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_assign_expr_res(char *dest);
/**
 * @brief      Generetes the addition of two floats or ints
 *
 * @param      op1   String with name of first var
 * @param      op2   String with name of second operator
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_add(char *op1, char *op2);
/**
 * @brief     Generetes the substraction of two floats or int
 *
 * @param      op1   String with name of first var
 * @param      op2   String with name of second operator
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_minus(char *op1, char *op2);
/**
 * @brief      Generetes the multiplication of two floats or ints
 *
 * @param      op1   String with name of first var
 * @param      op2   String with name of second operator
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_mult(char *op1, char *op2);
/**
 * @brief      Generetes the division of two floats
 *
 * @param      op1   String with name of first var
 * @param      op2   String with name of second var
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_div(char *op1, char *op2);
/**
 * @brief      Generetes the integer division of two integers
 *
 * @param      op1   String with name of first var
 * @param      op2   String with name of second var
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_idiv(char *op1, char *op2);
/**
 * @brief      Generetes the cocatenation of two strings
 *
 * @param      op1   String with name of first var
 * @param      op2   String with name of second var
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_concat(char *op1, char *op2);

//----------------------------------------
//                WHILE                  -
//----------------------------------------

/**
 * @brief      Generates label for while, from this point the body of while begins
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_while_label();
/**
 *
 * @brief      Generates beginning of while, the conditional jump to the end in
 *             case of condition ot being true anymore
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error 
 *             occurrence
 */
int gen_while_begin();
/**
 * @brief      Generates JUMP to the label of while and LABEL behind the while
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_while_end();


// ---------------------------------
//            FUNKCIE
// ---------------------------------

// @brief      Generates beginning of function, LABEL, PUSHFRAME, DEFVAR retval
//
// @param      id    String with name of function
//
// @return     OK if successfull, INTERNAL_ERROR in case of internal error
//             occurrence
int gen_f_start(char *id);
/**
 * @brief      Generates commands at the the end of function, POPFRAME, RETURN
 *
 * @param      id    String with name of function
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error occurrence
 */
int gen_f_end(char *id);
/**
 * @brief      Generates function call, CALL 'function label'
 *
 * @param      id    String with name of function
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error occurrence
 */
int gen_f_call(char *id);
/**
 * @brief      Generates commands to pass variable to function as parameters,
 *             CREATEFRAME, DEFVAR params
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_f_prep_params();

/**
 * @brief      Generates code to store the return of function to the variable
 *
 * @param      var   String containing variable name
 *
 * @return     OK if successfull, INTERNAL_ERROR in case of internal error
 *             occurrence
 */
int gen_f_return(char* var);


// unique labely
// na konci prejst zoznam a prehladat pouzitie built in funcii, tlacit len pouzite

#endif //_GENERATOR_H
