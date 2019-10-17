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

#define DEFAULT_STR_LEN 100

/**
 * @enum States in finite deterministic automata
 */
typedef enum
{
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
typedef enum
{
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
	KW_INT,
	KW_FLOAT,
	KW_STR,
} Keywords;

/**
 * @enum Tokens for syntax analysis
 */
typedef enum{
	TK_EOF,
	TK_EOL,
	TK_ID, //identifier
	TK_KW, //keyword

	// TODO

	// operators
} Tokens;


int get_next_token(); // poriesit odovzdavanie tokenov ...
int indentify(str* s); // zisti ci je string keyword

#endif //_SCANNER_H