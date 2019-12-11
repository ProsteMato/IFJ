/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @file expression_stack.h
 * @brief Stack for expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */

#ifndef _EXPRESSION_STACK_H
#define _EXPRESSION_STACK_H

#include "expression_parser.h"

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
int sPush(stackTop* eStack, pTable symbol, Data_type dType);

/**
 * @brief Pop a symbol from a stack 
 *
 * @param eStack stack 
 */
int sPop(stackTop* eStack);

#endif //_EXPRESSION_STACK_H