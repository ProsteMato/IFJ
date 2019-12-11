/**
 * @file parser.h
 * @author Martin Koči (xkocim05@stud.fit.vutbr.com)
 * 
 * @brief Header file of parser for syntactic analysis of programming language IFJ19
 * 
 * @date 2019-11-15
 * 
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "scanner.h"
#include "expression_parser.h"
#include "symtable.h"
#include "generator.h"
#include "sematic_analysis.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


/**
 * @brief      Representing rule <prog>
 * 	
 * It will be called at the begining of parser.
 * It will forward control to function st-list if the token is Keyword, identifier or expresstion
 * It will end the program as correct if the token is EOF.
 * Otherwise is a syntax error
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int prog(Token *token);

/**
 * @brief      
 * 	Representing rules: <st-list>, <nested-st-list>, <func-nested-st-list>
 * 	Diff is in they scope:
 * 	- st-list = main body of source code
 * 	- nested-st-list = if-else statement
 * 	- func-nested-st-list = in function
 *  It will forward controle to function stat if token was keyword, identifier or expresstion
 *  Otherwise is a syntax error
 * 
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int st_list(Token *token);

/**
 * @brief      
 * 	Representing rules: <stat>, <nested-stat>, <func-nested-stat>
 * 	Diff is in they scope:
 * 	- st-list = main body of source code
 * 	- nested-st-list = if-else statement
 * 	- func-nested-st-list = in function
 *  It will loop in this function if it will be identifier, keyword or expression it need to end with EOL
 *  and continue in this function or end the program.
 *  
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int stat(Token *token);

/**
 * @brief      Checking parameters of function in declaration
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int params(Token *token);

/**
 * @brief      Checking parameters of function in declaration
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int params_next(Token *token);

/**
 * @brief      Checking parameters of function when is called
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int arg_params(Token *token);

/**
 * @brief      Checking parameters of function when is called
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int arg_next_params(Token *token);

/**
 * @brief      Check parameters of the function for which type it is it will be usefull for generating
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int value(Token *token);

/**
 * @brief      Assigining the return value of function or identifier or expresttion
 *
 * @param      token  Token from scanner
 *
 * @return     Error code
 */
int assign(Token *token);

/**
 * @brief       This is checking what is after ID if is Left Bracket, operator, assign, EOL..
 * 
 * @param       token   Token from scanner
 * @return      Error code
 */
int after_id(Token *token);

/**
 * @brief       This is checking if token is ID or FUNCTION_ID
 * 
 * @param       token   Token from scanner
 * @return      Error code 
 */
int def_id(Token *token);

/**
 * @brief       This is checking if token is EOL or EXPR
 * 
 * @param       token   Token from scanner
 * @return      Error code
 */
int after_return(Token *token);

/**
 * @brief       This function check if on eol is EOL or EOF
 * 
 * @param       token   Token from scanner
 * @return      Error code
 */
int eof_or_eol(Token *token);

/**
 * @brief       Can be called only in function or if or while it is checking if next token is
 *              DEDENT or something else.
 * 
 * @param       token   Token from scanner
 * @return      Error code
 */
int next_st_list(Token *token);


int expression(Token *token);
#endif