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
		31: <nested-st-list> -> <nested-stat> EOL <nested-st-list>
		23: <func-nested-st-list> -> <func-nested-stat> EOL <func-nested-st-list>
	*/
	int returnValue = 0;
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
		} else if (strcmp(token->attribute, "if") == 0) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_EOL) {
					GET_NEXT_TOKEN(token);
					return st_list(token);
				}
			}
		} else if (strcmp(token->attribute, "while") == 0) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_EOL) {
					GET_NEXT_TOKEN(token);
					return st_list(token);
				}
			}
		} else if (strcmp(token->attribute, "pass") == 0) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_EOL) {
					GET_NEXT_TOKEN(token);
					return st_list(token);
				}
			}
		} else if (strcmp(token->attribute, "return") == 0 && in_function) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_EOL) {
					GET_NEXT_TOKEN(token);
					return st_list(token);
				}
			}
		}
	}

	/*
		37: <nested-st-list> -> DEDENT
		30: <func-nested-stat> -> DEDENT
	*/

	else if (token->type == TK_DEDENT) {
		if (in_if_while || in_function) {
			return OK;
		} else if (in_function && !in_if_while && depth == 0) {
			
			/* TODO bude treba opravit este pre dogenerovanie funkcie následne
			bude treba pridat asi lepšie kontrolu na dedent pre funckiu.
			*/

			return OK;
		}
	} else if (token->type == TK_ID) {
		/* TODO bude treba pridať asi pravdidlo na to že sa funkcia len tak zavolá
		a nebude do ničoho priradená následne pridať sématické akcie k tomuto.*/
		returnValue = stat(token);
		if (returnValue == OK) {
			GET_NEXT_TOKEN(token);
			if (token->type == TK_EOL) {
				GET_NEXT_TOKEN(token);
				return st_list(token);
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

int stat(Token *token) {
	;
}

int params(Token *token) {
	;
}

int params_next(Token *token) {
	;
}