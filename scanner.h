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

// Scanner states.
#define SCANST_START 0 // Starting state of every token
#define SCANST_EOL 1 // End of line
#define SCANST_COMM 2 // Line commentary until the SCANST_EOL
#define SCANST_COMM_BLOCK 3 // 
#define SCANST_COMM_BLOCK_EXIT 4 //
#define SCANST_START_OF_KW_ID 5 //
#define SCANST_NUMBER 6 //
#define SCANST_ 7 //

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