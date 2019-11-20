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
  PT_EOL, // 18 
  PT_SHIFT, //19 <
  PT_REDUCE, // 20 >  
  PT_EQUAL, // 21 = 
  PT_ERROR, //22 #
} pTable;

/**
 * @brief Function return an index in precedence table of token 
 * 
 * @param token token, which index I'm getting
 * 
 * @return index in precedence table  
 *          error code 
 */
int getIndex(Token *token) ;

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