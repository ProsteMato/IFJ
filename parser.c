/**
 * @file parser.c
 * @author Martin Koči (xkocim05@stud.fit.vutbr.com)
 * 
 * @brief Implementation of parser for syntactic analysis of programming language IFJ19
 * 
 * @date 2019-11-15
 * 
 */

#include "parser.h"


#define GET_NEXT_TOKEN(token) if (get_next_token(0, token) == LEX_ERROR) return LEX_ERROR
#define PRELOAD_NEXT_TOKEN(token) if (get_next_token(1, token) == LEX_ERROR) return LEX_ERROR

bool in_function = false;
bool in_if_while = false;
int depth = 0;

int prog(Token *token) {

	/*
	+----------------------+
  	|1: <prog> -> <st-list>|
  	+----------------------+ 
	*/
	if (token->type == TK_KW) {
		if(strcmp(token->attribute, "if") == 0) {

			return st_list(token);

		} else if (strcmp(token->attribute, "def") == 0) {

			return st_list(token);

		} else if (strcmp(token->attribute, "pass") == 0) {

			return st_list(token);

		} else if (strcmp(token->attribute, "while") == 0) {

			return st_list(token);

		} else if (strcmp(token->attribute, "None") == 0) {

			return st_list(token);

		} else {

			return SYNTAX_ERROR;

		}
	} else if (token->type == TK_ID || token->type == TK_STRING || 
			   token->type == TK_FLOAT || token->type == TK_INT || 
			   token->type == TK_BRACKET_L) {

		return st_list(token);

	} else if (token->type == TK_EOF) {

		return OK;

	} else {

		return SYNTAX_ERROR;

	}
}

int st_list(Token *token) {
	
	/* TODO opravit podľa novej LL tabulky a gramatiky
		2: <st-list> -> <stat> EOL <st-list>
		23: <func-nested-st-list> -> <func-nested-stat> EOL <next-func-nested-st-list>
		31: <nested-st-list> -> <nested-stat> EOL <next-nested-st-list>
	*/
	int returnValue = SYNTAX_ERROR; //TODO mozno bude treba zmenit...
	if (token->type == TK_KW) {
		if (strcmp(token->attribute, "def") == 0 && !in_function && !in_if_while) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_EOL) {
					GET_NEXT_TOKEN(token);
					return st_list(token);
				}
			}
		} else if (
				(strcmp(token->attribute, "if") == 0) ||
				(strcmp(token->attribute, "while") == 0) ||
				(strcmp(token->attribute, "pass") == 0) ||
				(strcmp(token->attribute, "return") == 0 && in_function)
				) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_EOL) {
					GET_NEXT_TOKEN(token);
					if (!in_function && !in_if_while) {
						return st_list(token);
					} else {
						return next_st_list(token);
					}
				}
			}
		} 
	} else if (token->type == TK_ID) {
		returnValue = stat(token);
		if (returnValue == OK) {
			GET_NEXT_TOKEN(token);
			if (token->type == TK_EOL) {
				GET_NEXT_TOKEN(token);
				if (!in_function && !in_if_while) {
					return st_list(token);
				} else {
					return next_st_list(token);
				}
			}
		}
	}

	/*
		3: <st-list> -> EOF
	*/
	 else if (token->type == EOF) {
		// TODO možno bude treba ešte upraviť teraz som si neni istý
		return OK;
	} else if (token->type == TK_ID || token->type == TK_STRING || 
			   token->type == TK_FLOAT || token->type == TK_INT || 
			   token->type == TK_BRACKET_L) {

		return stat(token);
	} else {
		return SYNTAX_ERROR;
	}
	return returnValue;
}

int next_st_list(Token *token) {

	/*
		38:  <next-nested-st-list> -> DEDENT
		39:  <next-nested-st-list> -> <nested-st-list>
		30:  <next-func-nested-st-list> -> DEDENT
		31:  <next-func-nested-st-list> -> <func-nested-st-list>
	*/
	if (token->type == TK_DEDENT) {
		if (in_if_while || in_function) {
			return OK;
		} else if (in_function && !in_if_while && depth == 0) {
			/* TODO bude treba opravit este pre dogenerovanie funkcie následne
			bude treba pridat asi lepšie kontrolu na dedent pre funckiu.
			*/
			return OK;
		}
	} else {
		return st_list(token);
	}
}

int stat(Token *token) {
	
}

int params(Token *token) {
	;
}

int params_next(Token *token) {
	;
}