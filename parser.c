#include "parser.h"


int prog(Token *token) {
	if (token->type == TK_KW) {
		if(strcmp(token->attribute, "if")) {

			return st_list(token);

		} else if (strcmp(token->attribute, "def")) {

			return st_list(token);

		} else if (strcmp(token->attribute, "pass")) {

			return st_list(token);

		} else if (strcmp(token->attribute, "while")) {

			return st_list(token);

		} else if (strcmp(token->attribute, "None")) {

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
	;
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