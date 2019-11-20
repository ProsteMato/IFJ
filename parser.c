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
		//TODO skontrolovat este neskor potom tento if else statementy či sú spravne
		if (in_if_while || in_function) {
			depth--;
			if (in_function && in_if_while && depth == 1) {
				in_if_while = false;
			} else if (in_function && !in_if_while && depth == 0) {
				in_function = false;
			} else if (!in_function && in_if_while && depth == 0) {
				in_if_while = false;
			}
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
	int returnValue = 0;
	
	if (token->type == TK_KW) {
		//4:  <stat> -> def id ( <params> : EOL INDENT <func-nested-st-list>
		if (strcmp(token->attribute, "def") == 0 && !in_function && !in_if_while) {
			GET_NEXT_TOKEN(token);
			if (token->type == TK_ID) {
				//TODO urobit nieco z ID pre novú definiciu funkcie
				GET_NEXT_TOKEN(token);
				if (token->type == TK_BRACKET_L) {
					returnValue = params(token);
					if (returnValue == OK) {
						GET_NEXT_TOKEN(token);
						if (token->type == TK_COLON) {
							GET_NEXT_TOKEN(token);
							if (token->type == TK_EOL) {
								GET_NEXT_TOKEN(token);
								if(token->type == TK_INDENT) {
									depth++;
									in_function = true;
									return st_list(token);
								}
							}
						}
					}
				}
			}
		 
		//9:  <stat> -> if expr : EOL INDENT <nested-st-list> else : EOL <nested-st-list>
		} else if (strcmp(token->attribute, "if") == 0) {
			GET_NEXT_TOKEN(token);
			//TODO priprava na generovanie...ziskat lables for if and else
			//TODO urobit samotne generovanie...
			if ((returnValue = callExpression(token)) == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_COLON) {
					GET_NEXT_TOKEN(token);
					if (token->type == TK_EOL) {
						GET_NEXT_TOKEN(token);
						if (token->type == TK_INDENT) {
							depth++;
							GET_NEXT_TOKEN(token);
							in_if_while = true;
							returnValue = st_list(token);
							if (returnValue == OK) {
								GET_NEXT_TOKEN(token);
								if (token->type == TK_KW && strcmp(token->attribute, "else") == 0) {
									GET_NEXT_TOKEN(token);
									if (token->type == TK_COLON) {
										GET_NEXT_TOKEN(token);
										if (token->type == TK_EOL) {
											GET_NEXT_TOKEN(token);
											if (token->type == TK_INDENT) {
												GET_NEXT_TOKEN(token);
												depth++;
												return st_list(token);
											}
										}
									}
								}
							} else {
								return returnValue;
							}
						}
					}
				}
			} else {
				return returnValue;
			}
		//8:  <stat> -> while expr : EOL INDENT <nested-st-list>
		} else if (strcmp(token->attribute, "while")) {
			//TODO generovanie while ziskavanie uniq lable...
			if ((returnValue = callExpression(token)) == OK) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_COLON) {
					GET_NEXT_TOKEN(token);
					if (token->type == TK_EOL) {
						GET_NEXT_TOKEN(token);
						if (token->type == TK_INDENT) {
							depth++;
							GET_NEXT_TOKEN(token);
							in_if_while = true;
							return st_list(token);
						}
					}
				}
			} else {
				return returnValue;
			}
		} else if(true) {
			//TODO pridat pass
			;
		}
	}
}

int params(Token *token) {
	;
}

int params_next(Token *token) {
	;
}