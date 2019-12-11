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
#include "main.h"

#define UNGET_TOKEN(token) \
		if ((token_return_value = unget_token((token))) != OK) \
			return token_return_value

#define PRELOAD_TOKEN(token) \
		if ((token_return_value = preload_token(token)) != OK) \
			return token_return_value

#define GET_NEXT_TOKEN(token) \
		if ((token_return_value = get_next_token((token))) != OK) \
			return token_return_value

bool in_function = false;
bool in_if_while = false;
int token_return_value = OK;
int depth = 0;
char *function_id = NULL;
char *saved_id = NULL;
char *copy_id = NULL;

int prog(Token *token) {
	pq_init();
	generate = true;
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
				if((returnValue = gen_f_end(function_id)) != OK) {
					return returnValue;
				}
				function_id = NULL;
				saved_id = NULL;
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
				if((returnValue = gen_while_end()) != OK) {
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
			copy_id = NULL;
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
				in_while = false;
				in_if_while = false;
			} else if (in_function && !in_if_while && depth == 0) {
				param_list = NULL;
				local_table = NULL;
				in_function = false;
			} else if (!in_function && in_if_while && depth == 0) {
				in_while = false;
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
				saved_id = token->attribute;
				function_id = token->attribute;
				if((returnValue = gen_f_start(saved_id)) != OK){
					return returnValue;
				}
				if(is_global_variable(root, saved_id)) {
					return SEM_FUNCTION_ERROR;
				}
				if((is_function_created(root, saved_id) && is_function_defined(root, saved_id)) ||
				(!is_function_created(root, saved_id) && !is_function_defined(root, saved_id))){
					returnValue = define_function(&root, saved_id);
					if(returnValue != OK) {
						return returnValue;
					}
				}
				local_table = FindLocalTable(root, saved_id);
				param_list = FindParamList(root, saved_id);
				GET_NEXT_TOKEN(token);
				if (token->type == TK_BRACKET_L) {
					returnValue = params(token);
					if (returnValue == OK) {
						SetDefine(root, saved_id);
						GET_NEXT_TOKEN(token);
						if (token->type == TK_COLON) {
							GET_NEXT_TOKEN(token);
							if (token->type == TK_EOL) {
								GET_NEXT_TOKEN(token);
								if(token->type == TK_INDENT) {
									depth++;
									in_function = true;
									GET_NEXT_TOKEN(token);
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
			in_if_while = true;
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
									in_if_while = true;
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
			in_if_while = true;
			if((returnValue = gen_while_label()) != OK) {
				return returnValue;
			}
			GET_NEXT_TOKEN(token);
			if ((returnValue = callExpression(token)) == OK) {
				if((returnValue = gen_while_begin()) != OK) {
					return returnValue;
				}
				GET_NEXT_TOKEN(token);
				if (token->type == TK_COLON) {
					GET_NEXT_TOKEN(token);
					if (token->type == TK_EOL) {
						GET_NEXT_TOKEN(token);
						if (token->type == TK_INDENT) {
							depth++;
							GET_NEXT_TOKEN(token);
							return st_list(token);
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
			int returnValue = 0;
			GET_NEXT_TOKEN(token);
			if((returnValue = after_return(token)) == OK) {
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
		UNGET_TOKEN(token);
		PRELOAD_TOKEN(token);
		if (
			token->type == TK_MINUS ||
			token->type == TK_PLUS ||
			token->type == TK_MULT ||
			token->type == TK_DIV ||
			token->type == TK_DIV_DIV
			) {
			generate = false;
			GET_NEXT_TOKEN(token);
			if((returnValue = callExpression(token)) == OK) {
				generate = true;
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
		generate = false;
		if((returnValue = callExpression(token)) == OK) {
			generate = true;
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
		if(WasCalled(root, saved_id)){
			return check_function_param_count(root, saved_id, 0);
		} else {
			SetParamCount(root, saved_id, 0);
		}
		return OK;
	} else if (token->type == TK_ID) {
		if(is_function_created(root, token->attribute)) {
			return SEM_FUNCTION_ERROR; //chyba redefinacia funkcie v parametri
		} 
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
			if(is_function_created(root, token->attribute)) {
				return SEM_FUNCTION_ERROR; //chyba redefinacia funkcie v parametri
			}
			int returnValue = ParamInsert(param_list, token->attribute);
			if (returnValue != OK) {
				return returnValue;
			}
			GET_NEXT_TOKEN(token);
			count++;
			return params_next(token);
		}
	} else if (token->type == TK_BRACKET_R) {
		if(WasCalled(root, saved_id)){
			int returnValue = check_function_param_count(root, saved_id, count);
			return returnValue;
		} else {
			SetParamCount(root, saved_id, count);
		}
		count = 1;
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
		if(in_function) {
			if(!is_function_defined(root, saved_id)) {
				if(!is_function_created(root, saved_id)) {
					if(is_global_variable(root, saved_id)) {
						return SEM_FUNCTION_ERROR;
					}
					returnValue = define_function(&root, saved_id);
					if (returnValue != OK) {
						return returnValue;
					}
					SetParamCount(root, saved_id, 0);
					SetCalled(root, saved_id);
				}
				return OK;
			}
		}
		if(is_function_created(root, saved_id)) {
			if(strcmp(saved_id, "print") != 0) {
				return check_function_param_count(root, saved_id, 0);
			} else {
				return OK;
			}
		} else {
			return SEM_FUNCTION_ERROR;
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
		if(in_function) {
			if(!is_function_defined(root, saved_id) && !is_function_created(root, saved_id)) {
				if(!is_function_created(root, saved_id)) {
					if(is_global_variable(root, saved_id)) {
						return SEM_FUNCTION_ERROR;
					}
					returnValue = define_function(&root, saved_id);
					if (returnValue != OK) {
						return returnValue;
					}
					SetParamCount(root, saved_id, count);
					SetCalled(root, saved_id);
				}
				count = 1;
				return OK;
			}
		}
	 	if(is_function_created(root, saved_id)){
			if(strcmp(saved_id, "print") != 0) {
				returnValue = check_function_param_count(root, saved_id, count);
				count = 1;
				return returnValue;
			} else {
				count = 1;
				return OK;
			}
		} else {
			return SEM_FUNCTION_ERROR;
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
			GET_NEXT_TOKEN(token);
			returnValue = def_id(token);
			if (returnValue != OK) {
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
			if((returnValue = gen_f_prep_params()) != OK) {
				return returnValue;
			}
			if((returnValue = gen_f_call(saved_id)) != OK) {
				return returnValue;
			}
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
		//copy_id = NULL;
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
			if((returnValue = gen_return()) != OK) {
				return returnValue;
			}
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
				return OK;
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
			return SEM_FUNCTION_ERROR;
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
