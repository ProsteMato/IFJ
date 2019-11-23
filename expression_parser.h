/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */


#ifndef _EXPRESSION_PARSER_H
#define _EXPRESSION_PARSER_H

#include "scanner.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include "symtable.h"
#include "expression_stack.h"


#define tableSize 19 
bool isRelational;  //is operator relational? <, <=, >, >=, ==, != 

typedef enum {
  PT_PLUS,  // 0
  PT_MINUS, // 1    
  PT_MULT, // 2
  PT_DIV,  // 3
  PT_DIV_DIV, // 4
  PT_LESS, // 5
  PT_LESS_EQ, // 6
  PT_GREAT, // 7 
  PT_GREAT_EQ, // 8 
  PT_EQ,  // 9 
  PT_NOT_EQ, // 10 
  PT_LEFT_BRACK, // 11
  PT_RIGHT_BRACK, // 12 
  PT_ID, // 13 
  PT_INT, // 14 
  PT_FLOAT, // 15
  PT_STRING, // 16 
  PT_NONE, // 17 
  PT_DOLLAR, // 18  //changed from PT_EOL
  PT_SHIFT, //19 <
  PT_REDUCE, // 20 >  
  PT_EQUAL, // 21 = 
  PT_ERROR, //22 #
  PT_E,
} pTable;

typedef enum {
  PR_EPLUSE,   // E-> E + E 
  PR_EMINUSE, // E-> E - E 
  PR_EMULTE,  // E-> E * E
  PR_EDIVE,   // E-> E / E 
  PR_EDIVDIVE, // E-> E // E 
  PR_ELESSE,   // E-> E < E 
  PR_ELESSEQE, // E-> E <= E 
  PR_EGREATE,  // E-> E > E 
  PR_EGREATEQE, // E-> E >= E 
  PR_EEQE,     // E-> E == E 
  PR_ENOTEQE,  // E-> E != E 
  PR_BIB,     // E-> (i)
  PR_OPERAND, // E-> i 
  PR_INT,     // E-> int 
  PR_FLOAT,   // E-> float
  PR_STRING,  // E-> string 
  PR_NONE,    // E-> None 
  PR_NOTARULE, // not a rule 
} pRules;

/**
 * @brief Function return an index in precedence table of token 
 * 
 * @param token token, which index I'm getting
 * 
 * @return index in precedence table  
 *          error code 
 */
int getIndex(Token* token) ;

/**
 * @brief Function to find data type of token 
 * 
 * @param token token, which data type we want to know 
 * 
 * @return TYPE_INT if it was integer number
 *         TYPE_FLOAT if it was float number
 *         TYPE_STRING if it was string 
 *         TYPE_NONE if it wasn't defined or called function when it shouldnt 
 */
data_type getDataType(Token *token);

/**
 * @brief Function checks data type of operands
 * 
 * @param pRules in which we are 
 * @param sym1 symbol on stack -- operand 1 (the newer on stack)
 * @param sym2 symbol on stack -- operator 
 * @param sym3 symbol on stack -- operand 2 (the older on stack )
 * 
 * @return OK if everything is okay 
 *          SEMATIC_ERROR if there was some sematic error
 */
int checkSematics(pRules rule, exprStack* sym1, exprStack* sym2, exprStack* sym3 );

/**
 * @brief Check if after / or // isnt next token 0 
 * 
 * @param token token / or // 
 *  
 * @return OK  if its not 0 
 *         DIVISION_BY_ZERO_ERROR error if division by zero 
 */
int checkDivisionByZero(Token *token);

/** 
 * @brief Function loads tokens into list and does some controls 
 * 
 * @param token first token 
 * @param error pointer to error, if there was some, there is SYNTAX_ERROR
 * 
 * @return eList list with loaded tokens
 */
exprList* createList(Token* token, int* error);

/**
 * @brief Functions finds rule for reducing 
 * 
 * @param num number of symbol to reduce on stack 
 * @param sym1 oldest symbol on stack 
 * @param sym2 next symbol on stack
 * @param sym3 next symbol on stack - top  
 * 
 * @return rule or PR_NOTARULE if there was none found 
 */
pRules findRule(int num, exprStack* sym1, exprStack* sym2, exprStack* sym3);

/**
 * @brief Function counts number of symbols to reduce by some rule
 * 
 * @return Numbers of symbol to reduce on stack (1 or 3)
 */
int symbolsToReduce()

/**
 * @brief Function that covers expression parser 
 * 
 * @param token token, that is handed to this function 
 * 
 * @return OK if everythings is right 
 *         SYNTAX_ERROR if there was some syntax error
 *         DIVISION_BY_ERROR if there was division by 0 
 *         ?? OTHER ERROR if there was another error
 */
int callExpression(Token *token);


#endif //_EXPRESSION_PARSER_H