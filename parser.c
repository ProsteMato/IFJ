/**
 * @file parser.c
 * @author Martin Koči (xkocim05@stud.fit.vutbr.com)
 * 
 * @brief Implementation of parser for syntactic analysis of programming language IFJ19
 * 
 * @date 2019-11-15
 * 
 */


//TODO SEM-A - kontrola pri definicii ID ci sa nerovná build-id function
//TODO SEM-A - na začatku programu definovať všetky build-id funcie
//TODO SEM-A - kontrolovať parametre build-id funkcii
//TODO SEM-A - skontrolovať každú definíciu premennej

#include "parser.h"
#include "main.h"

#define UNGET_TOKEN(token) \
		int token_return_value = 0; \
		if ((token_return_value = unget_token((token))) != OK) \
			return token_return_value

#define PRELOAD_TOKEN(token) \
		int token_return_value = 0; \
		if ((token_return_value = preload_token(token)) != OK) \
			return token_return_value

#define GET_NEXT_TOKEN(token) \
		int token_return_value = 0; \
		if ((token_return_value = get_next_token((token))) != OK) \
			return token_return_value

bool in_function = false;
bool in_if_while = false;
bool if_in_else = false;
int depth = 0;
Token savedToken;
char *saved_id = NULL;
char *copy_id = NULL;
char *funtion_id = NULL;

int prog(Token *token) {
	pq_init();
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
	
	/*
		2: <st-list> -> <stat> <st-list>
		23: <func-nested-st-list> -> <func-nested-stat> <next-func-nested-st-list>
		32:  <nested-st-list> -> <nested-stat> <next-nested-st-list>
	*/
	int returnValue = 0;
	if (token->type == TK_KW && !(strcmp(token->attribute, "None") == 0)) {
		
		if (strcmp(token->attribute, "def") == 0 && !in_function && !in_if_while) {
			returnValue = stat(token);
			if (returnValue == OK) {
				if((returnValue = gen_f_end(funtion_id)) != OK) {
					return returnValue;
				}
				funtion_id = NULL;
				GET_NEXT_TOKEN(token);
				return st_list(token);
			} else {
				return returnValue;
			}
		} else if (strcmp(token->attribute, "if") == 0) {
			returnValue = stat(token);
			if (returnValue == OK) {
				if((returnValue = gen_if_end()) != OK) {
					return returnValue;
				}
				GET_NEXT_TOKEN(token);
				if (!in_function && !in_if_while) {
					return st_list(token);
				} else {
					return next_st_list(token);
				}
			} else {
				return returnValue;
			}
		} else if (strcmp(token->attribute, "while") == 0) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (!in_function && !in_if_while) {
					return st_list(token);
				} else {
					return next_st_list(token);
				}
			} else {
				return returnValue;
			}
		} else if (strcmp(token->attribute, "pass") == 0) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (!in_function && !in_if_while) {
					return st_list(token);
				} else {
					return next_st_list(token);
				}
			} else {
				return returnValue;
			}
		} else if (strcmp(token->attribute, "return") == 0 && in_function) {
			returnValue = stat(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				if (!in_function && !in_if_while) {
					return st_list(token);
				} else {
					return next_st_list(token);
				}
			} else {
				return returnValue;
			}
		}
	} else if (token->type == TK_ID) {
		returnValue = stat(token);
		if (returnValue == OK) {
			GET_NEXT_TOKEN(token);
			if (!in_function && !in_if_while) {
				return st_list(token);
			} else {
				return next_st_list(token);
			}
		} else {
			return returnValue;
		}
	}

	/*
		3: <st-list> -> EOF
	*/
	 else if (token->type == TK_EOF) {
		char *key = UndefinedFunctionControl(root);
		if (key != NULL) {
			fprintf(stderr, "Funcia %s nieje definovaná\n", key);
			return SEM_FUNCTION_ERROR;
		}
		pq_destroy();
		return OK;
	} else if (token->type == TK_STRING || 
			   token->type == TK_FLOAT || token->type == TK_INT || 
			   token->type == TK_BRACKET_L) {

		return stat(token);
	}
	return SYNTAX_ERROR;
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
			depth--;
			if (in_function && in_if_while && depth == 1) {
				in_if_while = false;
			} else if (in_function && !in_if_while && depth == 0) {
				param_list = NULL;
				local_table = NULL;
				in_function = false;
			} else if (!in_function && in_if_while && depth == 0) {
				in_if_while = false;
			}
			return OK;
		}
	} else {
		return st_list(token);
	}
	return SYNTAX_ERROR;
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
				if((is_function_created(root, token->attribute) && is_function_defined(root, token->attribute)) ||
				   (!is_function_created(root, token->attribute) && !is_function_defined(root, token->attribute))){
					returnValue = define_function(&root, token->attribute);
					if(returnValue != OK) {
						return returnValue;
					}
				}
				funtion_id = token->attribute;
				if((returnValue = gen_f_start(funtion_id)) != OK){
					return returnValue;
				}
				saved_id = token->attribute;
				local_table = FindLocalTable(root, token->attribute);
				param_list = FindParamList(root, token->attribute);
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
									GET_NEXT_TOKEN(token);
									SetDefine(root, saved_id);
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
			if ((returnValue = callExpression(token)) == OK) {
				if((returnValue = gen_if()) != OK) {
					return returnValue;
				}
				GET_NEXT_TOKEN(token);
				if (token->type == TK_COLON) {
					GET_NEXT_TOKEN(token);
					if (token->type == TK_EOL) {
						GET_NEXT_TOKEN(token);
						if (token->type == TK_INDENT) {
							depth++;
							in_if_while = true;
							GET_NEXT_TOKEN(token);
							returnValue = st_list(token);
							if (returnValue == OK) {
								GET_NEXT_TOKEN(token);
								if (token->type == TK_KW && strcmp(token->attribute, "else") == 0) {
									if((returnValue = gen_else()) != OK) {
										return returnValue;
									}
									GET_NEXT_TOKEN(token);
									if (token->type == TK_COLON) {
										GET_NEXT_TOKEN(token);
										if (token->type == TK_EOL) {
											GET_NEXT_TOKEN(token);
											if (token->type == TK_INDENT) {
												GET_NEXT_TOKEN(token);
												depth++;
												in_if_while = true;
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
				printf("je to chyba v expr...\n");
				return returnValue;
			}
		/*
			8:  <stat> -> while expr : EOL INDENT <nested-st-list>
			27:  <func-nested-stat> -> while expr : EOL INDENT <func-nested-st-list>
			36:  <nested-stat> -> while expr : EOL INDENT <nested-st-list>
		*/
		} else if (strcmp(token->attribute, "while") == 0) {
			//TODO GEN-CODE = counter
			//TODO GEN-CODE = uniq lable
			GET_NEXT_TOKEN(token);
			if ((returnValue = callExpression(token)) == OK) {
				//TODO GEN-CODE = if podmienka bud skočit preč z while alebo ostat. treba vedieť lable na koniec while.
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
							//TODO GEN-CODE = lable na koniec while
						}
					}
				}
			} else {
				return returnValue;
			}
		/*
			7:  <stat> -> pass <eof-or-eol>
			24:  <func-nested-stat> -> pass <eof-or-eol>
			33:  <nested-stat> -> pass <eof-or-eol>
		*/
		} else if (strcmp(token->attribute, "pass") == 0) {
			GET_NEXT_TOKEN(token);
			if (token->type == TK_EOL || token->type == TK_EOF) {\
				return eof_or_eol(token);
			}
		/*
			29:  <func-nested-stat> -> return <after-return>
		*/		
		} else if (strcmp(token->attribute, "return") == 0 && in_function) {
			//TODO: GEN-CODE = return neviem ako to bude pracovat musim zistit bud vygenerujeme na začiatku funcie premennú ktorá bude return NULL
			// a potom ak sa objaví return tak to zmeníme a vygenerujeme uvidíme...
			int returnValue = 0;
			GET_NEXT_TOKEN(token);
			if((returnValue = after_return(token)) == OK) {
				//TODO GEN-CODE return
				return OK;
			} else {
				return returnValue;
			}
		}
	/*
		5:  <stat> -> id <after_id>
		28:  <func-nested-stat> -> id <after_id>
		37:  <nested-stat> -> id <after_id>
		26:  <func-nested-stat> -> expr <eof-or-eol>
		35:  <nested-stat> -> expr <eof-or-eol>
		6:  <stat> -> expr <eof-or-eol>
	*/
	} else if (token->type == TK_ID) {
		saved_id = token->attribute;
		PRELOAD_TOKEN(token);
		if (
			token->type == TK_MINUS ||
			token->type == TK_PLUS ||
			token->type == TK_MULT ||
			token->type == TK_DIV ||
			token->type == TK_DIV_DIV
			) {
			//TODO GEN-CODE - netreba generovať treba len vyhodnotit a vrátit OK
			if((returnValue = callExpression(token)) == OK) {
				if(!isRelational) {
					GET_NEXT_TOKEN(token);
					if (token->type == TK_EOL || token->type == TK_EOF) {
						return eof_or_eol(token);
					}
				} else {
					return SYNTAX_ERROR;
				}
			} else {
				return returnValue;
			}
		} else if (
				token->type == TK_EOL ||
				token->type == TK_EOF ||
				token->type == TK_BRACKET_L ||
				token->type == TK_ASSIGN
			) {
			GET_NEXT_TOKEN(token);
			return after_id(token);
		}
	/*
		26:  <func-nested-stat> -> expr	<eof-or-eol>
		35:  <nested-stat> -> expr <eof-or-eol>
		6:  <stat> -> expr <eof-or-eol>
	*/
	} else if (
		token->type == TK_FLOAT ||
		token->type == TK_STRING ||
		token->type == TK_INT ||
		(token->type == TK_KW && strcmp(token->attribute, "None") == 0)
		) {
		//TODO GEN-CODE - netreba generovať treba len vyhodnotit a vrátit OK
		if((returnValue = callExpression(token)) == OK) {
			if(!isRelational) {
				GET_NEXT_TOKEN(token);
				if (token->type == TK_EOL || token->type == TK_EOF) {
					return eof_or_eol(token);
				}
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
	GET_NEXT_TOKEN(token);
	if (token->type == TK_BRACKET_R) {
		if(is_function_defined(root, saved_id)){
			return check_function_param_count(root, saved_id, 0);
		} else {
			SetParamCount(root, saved_id, 0);
		}
		return OK;
	} else if (token->type == TK_ID) {
		int returnValue = ParamInsert(param_list, token->attribute);
		GET_NEXT_TOKEN(token);
		if (returnValue != OK) {
			return returnValue;
		}
		return params_next(token);
	} else {
		return SYNTAX_ERROR;
	}
}

/*
12:  <next-param> -> , id <next-param>
13:  <next-param> -> )
*/
int params_next(Token *token) {
	static int count = 1;
	if (token->type == TK_COMMA) {
		GET_NEXT_TOKEN(token);
		if (token->type == TK_ID) {
			int returnValue = ParamInsert(param_list, token->attribute);
			if (returnValue != OK) {
				return returnValue;
			}
			GET_NEXT_TOKEN(token);
			count++;
			return params_next(token);
		}
	} else if (token->type == TK_BRACKET_R) {
		if(is_function_defined(root, saved_id)){
			int returnValue = check_function_param_count(root, saved_id, count);
			count = 1;
			return returnValue;
		} else {
			SetParamCount(root, saved_id, count);
		}
		return OK;
	}
	return SYNTAX_ERROR;
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
		if(!is_function_defined(root, saved_id) && is_function_created(root, saved_id)) {
			if(!is_function_created(root, saved_id)) {
				returnValue = define_function(&root, saved_id);
				if (returnValue != OK) {
					return returnValue;
				}
				if(is_build_in_function(saved_id))
					set_build_in_function_param_count(root, saved_id);
					//TODO GEN-CODE vygenerovanie build in function
			} else if(is_global_variable(root, saved_id)) {
				return SEM_FUNCTION_ERROR;
			}
			SetParamCount(root, saved_id, 0);
			return OK;
		} else {
			return check_function_param_count(root, saved_id, 0);
		}
	} else {
		return SYNTAX_ERROR;
	}
}

/*
16:  <arg-next-params> -> , <value> <arg-next-params>
17:  <arg-next-params> -> )
*/
int arg_next_params(Token *token) {
	static int count = 1;
	int returnValue = 0;
	if (token->type == TK_COMMA) {
		GET_NEXT_TOKEN(token);
		if (token->type == TK_FLOAT || 
			token->type == TK_STRING || 
			token->type == TK_INT ||
			token->type == TK_ID ||
			(token->type == TK_KW && strcmp(token->attribute, "None") == 0)
		) {
			returnValue = value(token);
			if (returnValue == OK) {
				GET_NEXT_TOKEN(token);
				count++;
				return arg_next_params(token);
			} else {
				return returnValue;
			}
		}
	} else if (token->type == TK_BRACKET_R) {
		if(!is_function_defined(root, saved_id) && !is_function_created(root, saved_id)) {
			if(!is_function_created(root, saved_id)) {
				returnValue = define_function(&root, saved_id);
				if (returnValue != OK) {
					return returnValue;
				}
				if(is_build_in_function(saved_id))
					set_build_in_function_param_count(root, saved_id);
					//TODO GEN-CODE = vygenerovat build-in funckiu 
			} else if(is_global_variable(root, saved_id)) {
				return SEM_FUNCTION_ERROR;
			}
			SetParamCount(root, saved_id, count);
			count = 1;
			return OK;
		} else {
			returnValue = check_function_param_count(root, saved_id, count);
			count = 1;
			return returnValue;
		}
	}
	return SYNTAX_ERROR;
}

/*
42:  <assign> -> expr <eof-or-eol>
43:  <assign> -> id <def-id>
*/
int assign(Token *token) {
	int returnValue = 0;
	if (token->type == TK_BRACKET_L ||
		token->type == TK_FLOAT ||
		token->type == TK_INT ||
		token->type == TK_STRING ||
		(token->type == TK_KW && strcmp(token->attribute, "None") == 0)
		){
		GET_NEXT_TOKEN(token);
		if((returnValue = callExpression(token)) == OK) {
			if(!isRelational) {
				GET_NEXT_TOKEN(token);
				if(in_function){
					returnValue = define_local_variable(&local_table, false, copy_id);
					if (returnValue != OK){
						return returnValue;
					}
					LocalSetDefine(local_table, copy_id);
					LocalSetType(local_table, copy_id, finalType);
				} else {
					returnValue = define_global_variable(&root, false, copy_id);
					if (returnValue != OK){
						return returnValue;
					}
					SetDefine(root, copy_id);
					GlobalSetType(root, copy_id, finalType);
				}
				if (token->type == TK_EOL || token->type == TK_EOF) {
					return eof_or_eol(token);
				}
			} else {
				return SYNTAX_ERROR;
			}
		} else {
			return returnValue;
		}
	} else if (token->type == TK_ID) {
		saved_id = token->attribute;
		PRELOAD_TOKEN(token);
		if (token->type == TK_BRACKET_L) {
			GET_NEXT_TOKEN(token);
			returnValue = def_id(token);
			if (returnValue != OK) {
				return returnValue;
			}
			if((returnValue = gen_f_prep_params()) != OK) {
				return returnValue;
			}
			if((returnValue = gen_f_call(saved_id)) != OK) {
				return returnValue;
			}
			if(in_function){
				returnValue = define_local_variable(&local_table, true, copy_id);
				if (returnValue != OK){
					return returnValue;
				}
				LocalSetDefine(local_table, copy_id);
				LocalSetType(local_table, copy_id, TYPE_UNDEFINED);
			} else {
				returnValue = define_global_variable(&root, true, copy_id);
				if (returnValue != OK){
					return returnValue;
				}
				SetDefine(root, copy_id);
				GlobalSetType(root, copy_id, TYPE_UNDEFINED);
			}
			return OK;
		} else if (
			token->type == TK_PLUS ||
			token->type == TK_MINUS ||
			token->type == TK_MULT ||
			token->type == TK_DIV ||
			token->type == TK_DIV_DIV ||
			token->type == TK_EOL ||
			token->type == TK_EOF
		) {
			GET_NEXT_TOKEN(token);
			if((returnValue = callExpression(token)) == OK) {
				if(!isRelational) {
					if(in_function){
						//TODO SEM-A kontrola či lokálna premenná náhodou sa nerovná funkcii
						returnValue = define_local_variable(&local_table, false, copy_id);
						if (returnValue != OK){
							return returnValue;
						}
						LocalSetDefine(local_table, copy_id);
						LocalSetType(local_table, copy_id, finalType);
					} else {
						//TODO SEM-A kontrola či globálna premenná náhodou sa nerovná funkcii
						returnValue = define_global_variable(&root, false, copy_id);
						if (returnValue != OK){
							return returnValue;
						}
						SetDefine(root, copy_id);
						GlobalSetType(root, copy_id, finalType);
					}
					GET_NEXT_TOKEN(token);
					if (token->type == TK_EOL || token->type == TK_EOF) {
						return eof_or_eol(token);
					}
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
	if(token->type == TK_ASSIGN) {
		PRELOAD_TOKEN(token);
		copy_id = saved_id;
		return assign(token);
	} else if (token->type == TK_EOL || token->type == TK_EOF || token->type == TK_BRACKET_L) {
		return def_id(token);
	}
	return SYNTAX_ERROR;
}

/*
44:  <def-id> -> ( <arg-params> <eof-or-eol>
45:  <def-id> -> <eof-or-eol>
*/
int def_id(Token *token) {
	int returnValue = 0;
	if (token->type == TK_BRACKET_L) {
		GET_NEXT_TOKEN(token);
		if ((returnValue = arg_params(token)) == OK) {
			GET_NEXT_TOKEN(token);
			if (token->type == TK_EOL || token->type == TK_EOF) {
				return eof_or_eol(token);
			}
		} else {
			return returnValue;
		}
	} else if (token->type == TK_EOL || token->type == TK_EOF) {
		if(!is_variable_defined(root, local_table, param_list, saved_id)) {
			return SEM_FUNCTION_ERROR;
		}
		return eof_or_eol(token);
	} 
	return SYNTAX_ERROR;
}

/*
	46:  <after-return> -> expr <eof-or-eol>
	47:  <after-return> -> <eof-or-eol>
*/
int eof_or_eol(Token *token) {
	if (token->type == TK_EOL || token->type == TK_EOF) {
		copy_id = NULL;
		return OK;
	}
	return SYNTAX_ERROR;
}

/*
	48:  <eof-or-eol> -> EOL
	49:  <eof-or-eol> -> EOF
*/
int after_return(Token *token) {
	int returnValue = 0;
	if (token->type == TK_EOL || token->type == TK_EOF) {
		return eof_or_eol(token);
	}
	else if((returnValue = callExpression(token)) == OK) {
		if (!isRelational) {
			GET_NEXT_TOKEN(token);
			return eof_or_eol(token);
		} else {
			return SYNTAX_ERROR;
		}
	} 
	return returnValue;
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
		case TK_KW:
			if (strcmp(token->attribute, "None") == 0) {
				pq_queue(token, 0);
			}
			return OTHER_ERROR;
			break;
		case TK_FLOAT:
			pq_queue(token, 0);
			return OK;
			break;
		case TK_STRING:
			pq_queue(token, 0);
			return OK;
			break;
		case TK_INT:
			pq_queue(token, 0);
			return OK;
			break;
		case TK_ID:
			if (is_variable_defined(root, local_table, param_list, token->attribute)) {
				pq_queue(token, 0);
				return OK;
			}
			return SEM_PARAMS_ERROR;
			break;
		default:
			return OTHER_ERROR;
			break;
	}
}

int expression(Token *token) {
	while(token->type != TK_EOL && token->type != TK_COLON && token->type != TK_EOF) {
			GET_NEXT_TOKEN(token);
	}
	UNGET_TOKEN(token);
	return OK;
}