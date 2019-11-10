/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief Scanner interface.
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */
#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "error.h"

/**
 * @enum States in finite deterministic automata
 */
typedef enum{
	START,
	NUM,
	NUM_POINT,
	NUM_FLOAT,
	NUM_EXP,
	NUM_EXP_OPT,
	NUM_EXP_FIN,
	LINE_COMMENT,
	BLOCK_COMMENT0,
	BLOCK_COMMENT1,
	BLOCK_COMMENT2,
	BLOCK_COMMENT3,
	BLOCK_COMMENT4,
	STRING,
	STRING_ESCSEQ,
	STRING_HEX0,
	STRING_HEX1,
	STRING_FIN,
	EOL,
	ID_OR_KW,
	ID,
	COMMA,
	BRACKET_L,
	BRACKET_R,
	PLUS,
	MINUS,
	MULT,
	DIV,
	DIV_DIV,
	EQUAL,
	NOT_EQUAL,
	NEG,
	ASSIGN,
	LESSER,
	LESSER_EQUAL,
	GREATER,
	GREATER_EQUAL,
	COLON,
} States;

/**
 * @enum Keywords used in language IFJ19
 */
typedef enum{
	KW_DEF,
	KW_ELSE,
	KW_IF,
	KW_NONE,
	KW_PASS,
	KW_RETURN,
	KW_WHILE,
	KW_INPUTS,
	KW_INPUTI,
	KW_INPUTF,
	KW_PRINT,
	KW_LEN,
	KW_SUBSTR,
	KW_ORD,
	KW_CHR,
} Keywords;

/**
 * @enum Tokens for syntax analysis
 */
typedef enum{
	TK_EOF,
	TK_EOL,
	TK_ID,
	TK_KW,
	TK_COMMA,
	TK_BRACKET_L,
	TK_BRACKET_R,
	TK_PLUS,
	TK_MINUS,
	TK_MULT,
	TK_DIV,
	TK_DIV_DIV,
	TK_EQUAL,
	TK_NOT_EQUAL,
	TK_NEG,
	TK_ASSIGN,
	TK_LESSER,
	TK_LESSER_EQUAL,
	TK_GREATER,
	TK_GREATER_EQUAL,
	TK_COLON,
	TK_STRING,
	TK_INT,
	TK_FLOAT,
	TK_EMPTY,
} Tokens;

/**
 * @struct Token representation.
 */
typedef struct{
	Tokens type;
	char *atribute;
} Token;


/**
 * @brief      Runs the scanner and returns the next token
 *
 * @param      source  The source stream
 * @param      token   Pointer to the allocated token struct
 *
 * @return     OK-token is returned successfully, INTERNAL_ERROR-internal error, LEX_ERROR lex error
 */
int get_next_token(FILE *source, Token *token);

/**
 * @brief      Determines whether the string is keyword
 *
 * @param      s     Pointer to the string
 * @param[in]  len   The length of string
 *
 * @return     Type of keyword, KW_NONE in case the string is not keyword
 */
Keywords is_keyword (char* s, unsigned len);

#endif //_SCANNER_H