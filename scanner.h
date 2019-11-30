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
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "error.h"
#include "my_string.h"
#include "stack.h"

/**
 * @enum Tokens for syntax analysis
 */
typedef enum{
	TK_EOF,		    // 0
	TK_EOL,		    // 1
	TK_ID,		    // 2
	TK_KW, 		    // 3
	TK_COMMA, 		// 4 
	TK_BRACKET_L,   // 5
	TK_BRACKET_R,   // 6
	TK_PLUS,        // 7
	TK_MINUS, 		// 8
	TK_MULT, 		// 9
	TK_DIV, 		// 10
	TK_DIV_DIV, 	// 11
	TK_EQUAL, 		// 12
	TK_NOT_EQUAL, 	// 13
	TK_NEG, 		// 14
	TK_ASSIGN, 		// 15
	TK_LESSER, 		// 16
	TK_LESSER_EQUAL,// 17
	TK_GREATER, 	// 18
	TK_GREATER_EQUAL,// 19
	TK_COLON, 		// 20
	TK_STRING, 		// 21
	TK_INT, 		// 22
	TK_FLOAT, 		// 23
	TK_DEDENT, 		// 24
	TK_INDENT, 		// 25
	TK_SQR_L_BRACKET, // 26
	TK_SQR_R_BRACKET, // 27
} Tokens;

/**
 * @struct Token representation.
 */
typedef struct{
	Tokens type;
	char *attribute;
} Token;

typedef struct QToken QToken;

/**
 * @brief      Structure representing element in token queue
 */
struct QToken{
	Token token;
	int ret_val;
	QToken *behind;
};

/**
 * @struct Token queue
 */
typedef struct{
	QToken *last;
	QToken *first;
} TKQueue;

/**
 * @brief      Global variable for token queue
 */
TKQueue q;

/**
 * @enum States in finite deterministic automata
 */
typedef enum{
	START,				//0
	NUM,				//1
	NUM_POINT,			//2
	NUM_FLOAT,			//3
	NUM_EXP,			//4
	NUM_EXP_OPT,		//5
	NUM_EXP_FIN,		//6
	LINE_COMMENT,		//7
	BLOCK_COMMENT0,		//8
	BLOCK_COMMENT1,		//9
	BLOCK_COMMENT2,		//10
	BLOCK_COMMENT3,		//11
	BLOCK_COMMENT4,		//12
	STRING,				//13
	STRING_ESCSEQ,		//14
	STRING_HEX0,		//15
	STRING_HEX1,		//16
	STRING_FIN,			//17
	EOL,				//18
	ID_OR_KW,			//19
	ID,					//20
	COMMA,				//21
	BRACKET_L,			//22
	BRACKET_R,			//23
	PLUS,				//24
	MINUS,				//25
	MULT,				//26
	DIV,				//27
	DIV_DIV,			//28
	EQUAL,				//29
	NOT_EQUAL,			//30
	NEG,				//31
	ASSIGN,				//32
	LESSER,				//33
	LESSER_EQUAL,		//34
	GREATER,			//35
	GREATER_EQUAL,		//36
	COLON,				//37
	INDENTATION,		//38
} States;

/**
 * @enum Keywords used in language IFJ19
 */
typedef enum{
	KW_DEF,		//1
	KW_ELSE,	//2
	KW_IF,		//3
	KW_NONE,	//4
	KW_PASS,	//5
	KW_RETURN,	//6
	KW_WHILE,	//7
	KW_INPUTS,	//8
	KW_INPUTI,	//9
	KW_INPUTF,	//10
	KW_PRINT,	//11
	KW_LEN,		//12
	KW_SUBSTR,	//13
	KW_ORD,		//14
	KW_CHR,		//15
} Keywords;

/**
 * @brief      Return the next token
 *
 * @param      token  Pointer to the allocated token struct
 * @param      q      Pointer to the token queue
 *
 * @return     OK-token is returned successfully, INTERNAL_ERROR-internal error,
 *             LEX_ERROR lex error
 */
int get_next_token(Token *token);

/**
 * @brief      Determines whether the string is keyword
 *
 * @param      s     Pointer to the string
 * @param[in]  len   The length of string
 *
 * @return     Type of keyword, -1 in case the string is not keyword
 */
Keywords is_keyword (char* s, unsigned len);

/**
 * @brief      Destroys the stack, deallocates the string and returns
 *             INTERNAL_ERROR
 *
 * @param      s     Pointer to the stack struct
 * @param      str   Pointer to the string
 *
 * @return     INTERNAL_ERROR
 */
int internal_error_exit(Stack *s, char *str);


/**
 * @brief      Pushes the token to the token queue
 *
 * @param      token  The token
 * @param      q      Pointer to the token queue
 *
 * @return     OK-token is returned successfully, INTERNAL_ERROR-internal error,
 *             LEX_ERROR lex error
 */
int unget_token(Token *token);

/**
 * @brief      Scans the input and finds the token
 *
 * @param      token  Pointer to the token struct
 *
 * @return     OK-token is returned successfully, INTERNAL_ERROR-internal error,
 *             LEX_ERROR lex error
 */
int scan(Token *token);

/**
 * @brief      Preloads the next token
 *
 * @param      token  The pointer to the token
 *
 * @return      OK-token is returned successfully, INTERNAL_ERROR-internal error,
 *             LEX_ERROR lex error
 */
int preload_token(Token *token);

/**
 * @brief      Inicializes the token queue
 */
void tkq_init();

/**
 * @brief      Adds token to the back of the queue
 *
 * @param      token    The token
 * @param[in]  ret_val  The ret value
 *
 * @return     OK- token is added successfully, INTERNAL_ERROR-internal error
 */
int tkq_queue(Token *token, int ret_val);

/**
 * @brief      Removes the first token from the queue and returns it
 *
 * @param      token  The token pointer
 *
 * @return     ret_value of First token in queue, OK-token is returned
 *             successfully, INTERNAL_ERROR-internal error, LEX_ERROR lex error
 */
int tkq_dequeue(Token* token);

/**
 * @brief      Returns the first token in queue, doesnt remove it from queue
 *
 * @param      token  The token pointer
 *
 * @return     ret_value of First token in queue, OK-token is returned
 *             successfully, INTERNAL_ERROR-internal error, LEX_ERROR lex error
 */
int tkq_first(Token *token);

/**
 * @brief      Deallocates memory of queue
 */
void q_destroy();


#endif //_SCANNER_H
