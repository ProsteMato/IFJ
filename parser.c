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
		32:  <nested-st-list> -> <nested-stat> EOL <next-nested-st-list>
	*/
	int returnValue = SYNTAX_ERROR;
	if (token->type == TK_KW && !(strcmp(token->attribute, "None") == 0)) {
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
		/*
			4:  <stat> -> def id ( <params> : EOL INDENT <func-nested-st-list>
		*/
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
					} else {
						return returnValue;
					}
				}
			}
		 
		/*
			9:  <stat> -> if expr : EOL INDENT <nested-st-list> else : EOL <nested-st-list>
			25:  <func-nested-stat> -> if expr : EOL INDENT <func-nested-st-list> else : EOL INDENT <func-nested-st-list>
			34:  <nested-stat> -> if expr : EOL INDENT <nested-st-list> else : EOL INDENT <nested-st-list>
		*/
		} else if (strcmp(token->attribute, "if") == 0) {
			GET_NEXT_TOKEN(token);
			//TODO priprava na generovanie...ziskat lables for if and else
			//TODO urobit samotne generovanie...
			//if ((returnValue = callExpression(token)) == OK) {
			if ((returnValue = expresstion(token)) == OK) {
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
		/*
			8:  <stat> -> while expr : EOL INDENT <nested-st-list>
			27:  <func-nested-stat> -> while expr : EOL INDENT <func-nested-st-list>
			36:  <nested-stat> -> while expr : EOL INDENT <nested-st-list>
		*/
		} else if (strcmp(token->attribute, "while") == 0) {
			//TODO generovanie while ziskavanie uniq lable...
			//if ((returnValue = callExpression(token)) == OK) {
			if ((returnValue = expresstion(token)) == OK) {
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
		/*
			7:  <stat> -> pass
			24:  <func-nested-stat> -> pass
			33:  <nested-stat> -> pass
		*/
		} else if (strcmp(token->attribute, "pass") == 0) {
			return OK;
		/*
			29:  <func-nested-stat> -> return expr
		*/
		} else if (strcmp(token->attribute, "return") == 0 && in_function) {
			GET_NEXT_TOKEN(token);
			//if((returnValue = callExpression(token)) == OK) {
			if ((returnValue = expresstion(token)) == OK) {
				if (!isRelational) {
					return OK;
				} else {
					return SYNTAX_ERROR;
				}
			} else {
				return returnValue;
			}
		}
	/*
		5:  <stat> -> id <after_id>
		28:  <func-nested-stat> -> id <after_id>
		37:  <nested-stat> -> id <after_id>
		26:  <func-nested-stat> -> expr
		35:  <nested-stat> -> expr
		6:  <stat> -> expr
	*/
	} else if (token->type == TK_ID) {
		Token prelaod_token;
		PRELOAD_NEXT_TOKEN(&prelaod_token);
		if (
			prelaod_token.type == TK_MINUS ||
			prelaod_token.type == TK_PLUS ||
			prelaod_token.type == TK_MULT ||
			prelaod_token.type == TK_DIV ||
			prelaod_token.type == TK_DIV_DIV
			) {
			//if((returnValue = callExpression(token)) == OK) {
			if ((returnValue = expresstion(token)) == OK) {
				if(!isRelational) {
					return OK;
				} else {
					return SYNTAX_ERROR;
				}
			} else {
				return returnValue;
			}
		} else if (
				prelaod_token.type == TK_EOL || 
				prelaod_token.type == TK_BRACKET_L ||
				prelaod_token.type == TK_ASSIGN
			) {
			GET_NEXT_TOKEN(token);
			return after_id(token);
		}
	/*
		26:  <func-nested-stat> -> expr
		35:  <nested-stat> -> expr
		6:  <stat> -> expr
	*/
	} else if (
		token->type == TK_FLOAT ||
		token->type == TK_STRING ||
		token->type == TK_INT ||
		(token->type == TK_KW && strcmp(token->attribute, "None") == 0)
		) {
		//if((returnValue = callExpression(token)) == OK) {
		if ((returnValue = expresstion(token)) == OK) {
			if(!isRelational) {
				return OK;
			} else {
				return SYNTAX_ERROR;
			}
		} else {
			return returnValue;
		}	
	}
	return SYNTAX_ERROR;
}

/*
10:  <params> -> )
11:  <params> -> id <next-param>
*/
int params(Token *token) {
	//TODO spracovávanie kontrolovanie a generovanie funkcie
	GET_NEXT_TOKEN(token);
	if (token->type == TK_BRACKET_R) {
		return OK;
	} else if (token->type == TK_ID) {
		GET_NEXT_TOKEN(token);
		return params_next(token);
	} else {
		return SYNTAX_ERROR;
	}
}

/*
12:  <next-param> -> , <params>
13:  <next-param> -> )
*/
int params_next(Token *token) {
	//TODO pridat generovanie atd..
	if (token->type == TK_COMMA) {
		GET_NEXT_TOKEN(token);
		return params(token);
	} else if (token->type == TK_BRACKET_R) {
		return OK;
	} else {
		return SYNTAX_ERROR;
	}
}

/*
14:  <arg-params> -> )
15:  <arg-params> -> <value> <arg-next-params>
*/
int arg_params(Token *token) {
	int returnValue = 0;
	if (token->type == TK_FLOAT || 
		token->type == TK_STRING || 
		token->type == TK_INT ||
		token->type == TK_ID ||
		(token->type == TK_KW && strcmp(token->attribute, "None") == 0)
		) {
			returnValue = value(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				return arg_next_params(token);
			} else {
				return returnValue;
			}
	} else if (token->type == TK_BRACKET_R) {
		return OK;
	} else {
		return SYNTAX_ERROR;
	}
}

/*
16:  <arg-next-params> -> , <value> <arg-next-params>
17:  <arg-next-params> -> )
*/
int arg_next_params(Token *token) {
	//TODO pridat generovanie a kontrolovanie atd..
	int returnValue = 0;
	if (token->type == TK_COMMA) {
		GET_NEXT_TOKEN(token);
		if (token->type == TK_FLOAT || 
			token->type == TK_STRING || 
			token->type == TK_INT ||
			token->type == TK_ID ||
			(token->type == TK_KW && strcmp(token->attribute, "None") == 0)
		) {
			GET_NEXT_TOKEN(token);
			returnValue = value(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				return arg_next_params(token);
			} else {
				return OK;
			}
		}
	} else if (token->type == TK_BRACKET_R) {
		return OK;
	} else {
		return SYNTAX_ERROR;
	}
}

/*
42:  <assign> -> expr
43:  <assign> -> id <def-id>
*/
int assign(Token *token) {
	// TODO pridat generovanie, kontrolovanie atd...
	int returnValue = 0;
	if (token->type == TK_BRACKET_L ||
		token->type == TK_FLOAT ||
		token->type == TK_INT ||
		token->type == TK_STRING ||
		(token->type == TK_KW && strcmp(token->attribute, "None") == 0)
		){
		//if((returnValue = callExpression(token)) == OK) {
		if ((returnValue = expresstion(token)) == OK) {
			if(!isRelational) {
				return OK;
			} else {
				return SYNTAX_ERROR;
			}
		} else {
			return returnValue;
		}
	} else if (token->type == TK_ID) {
		Token preload_token;
		PRELOAD_NEXT_TOKEN(&preload_token);
		if (preload_token.type == TK_BRACKET_L || preload_token.type == TK_EOL) {
			return def_id(token);
		} else if (
			preload_token.type == TK_PLUS ||
			preload_token.type == TK_MINUS ||
			preload_token.type == TK_MULT ||
			preload_token.type == TK_DIV ||
			preload_token.type == TK_DIV_DIV
		) {
			//if((returnValue = callExpression(token)) == OK) {
			if ((returnValue = expresstion(token)) == OK) {
				if(!isRelational) {
					return OK;
				} else {
					return SYNTAX_ERROR;
				}
			} else {
				return returnValue;
			}
		} 
	} 
	return SYNTAX_ERROR;
}

/*
40:  <after_id> -> = <assign>
41:  <after_id> -> <def-id>
*/
int after_id(Token *token) {
	// TODO pridat generovanie atd..
	if(token->type == TK_ASSIGN) {
		GET_NEXT_TOKEN(token);
		return assign(token);
	} else if (token->type == EOL || token->type == TK_BRACKET_L) {
		return def_id(token);
	}
}

/*
44:  <def-id> -> ( <arg-params>
45:  <def-id> -> EOL
*/
int def_id(Token *token) {
	//TODO pridat generovanie atd...
	if (token->type == TK_BRACKET_L) {
		GET_NEXT_TOKEN(token);
		return arg_params(token);
	} else if (token->type == TK_EOL) {
		return OK;
	} else {
		return SYNTAX_ERROR;
	}
}


/*
18:  <value> -> none
19:  <value> -> float
20:  <value> -> string
21:  <value> -> int
22:  <value> -> id
*/
int value(Token *token) {
	switch(token->type) {
		//TODO spracovanie každeho parametru pri volaní funkcie.. 
		case TK_KW:
			return OK;
			break;
		case TK_FLOAT:
			return OK;
			break;
		case TK_STRING:
			return OK;
			break;
		case TK_INT:
			return OK;
			break;
		case TK_ID:
			return OK;
			break;
		default:
			return SYNTAX_ERROR;
			break;
	}
}

int expresstion(Token *token) {
	while(
		token->type != TK_EOL ||
		token->type != TK_COLON
		) {
			GET_NEXT_TOKEN(token);
	}
	return OK;
}