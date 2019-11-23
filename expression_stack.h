/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Stack for expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */

#ifndef _EXPRESSION_STACK_H
#define _EXPRESSION_STACK_H

#include "scanner.h"
#include "expression_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "error.h"

/**
 * @struct Structure to represent a stack
 * 
 * @param symbol represents symbol on the stack 
 * @param dType data type of symbol on stack (NONE if operand )
 * @param next next symbol on stack 
 */
typedef struct stack {
    pTable symbol;
    data_type dType; 
    struct stack *next; 
} exprStack; 


/**
 * @struct Structure, that represent the top symbol on stack 
 * 
 * @param top pointer to the top symbol of stack 
 */
typedef struct {
    exprStack *top; 
} stackTop;

/** 
 * @brief Initialization of stack 
 * 
 * @param  eStack stack to inizialize 
 * 
 */ 
int sInit(stackTop* eStack); 

/**
 * @brief Function return symbol on top of the stack 
 * 
 * @param eStack stack; 
 * 
 * @return struct of top symbol 
 */
exprStack* sTop(stackTop* eStack);


/**
 * @brief Push a symbol to top of the stack
 * 
 * @param eStack stack 
 * @param symbol symbol to push onto a stack
 * @param dType type of symbol 
 */
int sPush(stackTop* eStack, pTable symbol, data_type dType);

/**
 * @brief Pop a symbol from a stack 
 *
 * @param eStack stack 
 */
int sPop(stackTop* eStack);


/** 
 * @brief Function deletes stack
 * 
 * @param eStack stack to delete 
 * 
 */
void disposeStack(stackTop* eStack);



#endif //_EXPRESSION_STACK_H