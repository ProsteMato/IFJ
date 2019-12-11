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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include "symtable.h"
#include "sematic_analysis.h"
#include "generator.h"


#define tableSize 19 
extern bool isRelational;  //is operator relational? <, <=, >, >=, ==, != 
extern bool generate;
extern Data_type finalType;

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
  PT_E, // 23
} pTable;

/**
 * @struct Structure to represent a stack
 * 
 * @param symbol represents symbol on the stack 
 * @param dType data type of symbol on stack (NONE if operand )
 * @param next next symbol on stack 
 */
typedef struct stack {
    pTable symbol;
    Data_type dType; 
    struct stack *next; 
} exprStack; 

typedef struct listItem {           
        pTable symbol;  
        char *attribute;
        Data_type dType;                                       
        struct listItem *lptr;          
        struct listItem *rptr;        
} *item;

typedef struct {
    item first;
    item act;
    item last;
} exprList;



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

int precedenceRules[100] ;

#include "expression_list.h"
#include "expression_stack.h"

exprList operandList;

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
 * @brief Function checks data type of operands
 * 
 * @param rule in which we are 
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
int symbolsToReduce();

/** 
 * @brief Functin returs final type of operation
 * 
 * @param rule rule used for reduction 
 * @param sym1 symbol on stack
 * @param sym2 operation on stack
 * @param sym3 symbol on stack 
 * 
 * @return TYPE_INT if final type of operation is INT
 *          TYPE_FLOAT if final type of operation is FLOAT
 *          TYPE_STRING if final type of operation is STRING
 *          TYPE_BOOL if relational operation
 *          TYPE_UNDEFINED if some error
 */
Data_type getFinalType(pRules rule, exprStack* sym1, exprStack* sym2, exprStack* sym3);

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