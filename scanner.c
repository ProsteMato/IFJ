/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief Scanner implementation.
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "scanner.h"

Keywords is_keyword(char* s, unsigned len){
	if (strncmp (s, "def", len) == 0){
		return KW_DEF;
	} else if (strncmp (s, "else", len) == 0){
		return KW_ELSE;
	} else if (strncmp (s, "if", len) == 0){
		return KW_IF;
	} else if (strncmp (s, "None", len) == 0){
		return KW_NONE;
	} else if (strncmp (s, "pass", len) == 0){
		return KW_PASS;
	} else if (strncmp (s, "return", len) == 0){
		return KW_RETURN;
	} else if (strncmp (s, "while", len) == 0){
		return KW_WHILE;
	} else if (strncmp (s, "inputs", len) == 0){
		return KW_INPUTS;
	} else if (strncmp (s, "inputf", len) == 0){
		return KW_INPUTF;
	} else if (strncmp (s, "inputi", len) == 0){
		return KW_INPUTI;
	} else if (strncmp (s, "len", len) == 0){
		return KW_LEN;
	} else if (strncmp (s, "substr", len) == 0){
		return KW_SUBSTR;
	} else if (strncmp (s, "ord", len) == 0){
		return KW_ORD;
	} else if (strncmp (s, "chr", len) == 0){
		return KW_CHR;
	} else if (strncmp (s, "print", len) == 0){
		return KW_PRINT;
	} else {
		return -1;
	}
}

int internal_error_exit(Stack *s, char *str){
	free(str);
	s_destroy(s);
	return INTERNAL_ERROR;
}

int save_preload(Token *preloaded, Token *dest, int return_val, int *dest_return){
	dest->type = preloaded->type;
	dest->attribute = preloaded->attribute;
	*dest_return = return_val;
	return return_val;
}

int get_next_token(Token *token, int preload){
	// kontrola parametrov
	if (token == NULL){
		return INTERNAL_ERROR;
	}
	// preload
	static Token preloaded_token = {-1, NULL};
	static int preloaded_return;
	if (!preload && preloaded_token.type != -1) {
		// normalne pouzitie get_next_token ale mame uz preloadnuty token -> vratime preloadnuty
		token->type = preloaded_token.type;
		token->attribute = preloaded_token.attribute;
		preloaded_token.type = -1;
		return preloaded_return;
	} else if (preload && preloaded_token.type != -1){
		// preload po predoslom preloadnuti -> vraciame zase preloadnuty token
		token->type = preloaded_token.type;
		token->attribute = preloaded_token.attribute;
		return preloaded_return;
	}
	// inicializacia stacku
	static Stack s = {NULL, 0, -1,};
	if (s.size == 0){
		s_init(&s);
		if (s.data == NULL){
			return INTERNAL_ERROR;
		}
		s_push(&s, 0);
	}
	// inicializacia premennych
	char* str = mystring_init();
	if (str == NULL){
		// chyba alokacie pamate
		s_destroy(&s);
		return INTERNAL_ERROR;
	}
	static int first_token = 1;
	static int dedent_flag = 0;
	static unsigned int spc_counter = 0;
	int comment_eol = 0;
	unsigned long cap = DEFAULT_STR_LEN;
	unsigned long str_i = 0;
	int c;
	int state = START;
	int keyword = -1;
	char hex[2];
	int block_end = 0;
	if (dedent_flag){
		state = INDENTATION;
	}
	while(true){
		c = getc(stdin);
		switch(state){
			case (START):
			str_i = 0;
				if (isalpha(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					first_token = 0;
					state = ID_OR_KW;
				} else if (c == EOF){
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					token->type = TK_EOF;
					s_destroy(&s);
					if (preload){
						 return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}								
				} else if (isdigit(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					first_token = 0;
					state = NUM;
				} else if (c == ' '){
					if (first_token){
						state = INDENTATION;
						spc_counter++;
					} else {
						state = START;
					}
				} else if (c == '_'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					first_token = 0;
					state = ID;					
				} else if (c == '#'){
					state = LINE_COMMENT;				
				} else if (c == '"'){
					state = BLOCK_COMMENT0;						
				} else if (c == 39){ // ascii 39 == '
					first_token = 0;
					state = STRING;
				} else if (c == '/'){
					first_token = 0;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = DIV;										
				} else if (c == '='){
					first_token = 0;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = ASSIGN;
				} else if (c == '<'){
					first_token = 0;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = LESSER;
				} else if (c == '>'){
					first_token = 0;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = GREATER;
				} else if (c == '!'){
					first_token = 0;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NEG;
				} else if (c == '\n'){
					if (comment_eol && !first_token){
						ungetc(c, stdin);
						token->type = TK_EOL;
						spc_counter = 0;
						first_token = 1;
						if (!append_char(&str, &str_i, &cap, c)){
							return internal_error_exit(&s, str);
						}
						if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
						token->attribute = str;
						if (preload){
							return save_preload(token, &preloaded_token, OK, &preloaded_return);
						} else {
							return OK;
						}	
					}
					if (first_token){
						state = INDENTATION;
					} else {
						token->type = TK_EOL;
						spc_counter = 0;
						first_token = 1;
						ungetc(c, stdin);
						if (!append_char(&str, &str_i, &cap, c)){
							return internal_error_exit(&s, str);
						}
						if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
						token->attribute = str;
						if (preload){
							return save_preload(token, &preloaded_token, OK, &preloaded_return);
						} else {
							return OK;
						}	
					}
				} else if (c == '*'){
					first_token = 0;
					token->type = TK_MULT;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}	
				} else if (c == '+'){
					first_token = 0;
					token->type = TK_PLUS;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else if (c == '-'){
					first_token = 0;
					token->type = TK_MINUS;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else if (c == ':'){
					first_token = 0;
					token->type = TK_COLON;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else if (c == '('){
					first_token = 0;
					token->type = TK_BRACKET_L;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else if (c == ')'){
					first_token = 0;
					token->type = TK_BRACKET_R;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else if (c == ','){
					first_token = 0;
					token->type = TK_COMMA;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else if (c == '['){
					first_token = 0;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					token->type = TK_SQR_L_BRACKET;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else if (c == ']'){
					first_token = 0;
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					token->type = TK_SQR_R_BRACKET;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else {
					// lex chyba
					free(str);
					if (preload){
						return save_preload(token, &preloaded_token, LEX_ERROR, &preloaded_return);
					} else {
						return LEX_ERROR;
					}
				}
				break;
			case (ID):
				if (isalpha(c) || isdigit(c) || c == '_'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = ID;
				} else {
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_ID;
					token->attribute = str;
					ungetc(c, stdin);
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}				}
				break;
			case (ID_OR_KW):
				if (isalpha(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					keyword = is_keyword(str, str_i);
					state = ID_OR_KW;
				} else if (isdigit(c) || c == '_'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = ID;
				} else {
					if (keyword >= 0){
						if (!append_char(&str, &str_i, &cap, '\0')){
							return internal_error_exit(&s, str);
						}
						token->type = TK_KW;
						token->attribute = str;
						ungetc(c, stdin);
						if (preload){
							return save_preload(token, &preloaded_token, OK, &preloaded_return);
						} else {
							return OK;
						}
					} else {
						if (!append_char(&str, &str_i, &cap, '\0')){
							return internal_error_exit(&s, str);
						}
						token->type = TK_ID;
						token->attribute = str;
						ungetc(c, stdin);
						if (preload){
							return save_preload(token, &preloaded_token, OK, &preloaded_return);
						} else {
							return OK;
						}
					}
				}
				break;
			case (NUM):
				if (isdigit(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM;
				} else if (c == '.'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_POINT;
				} else if (c == 'E' || c == 'e'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_EXP;
				} else {
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_INT;
					token->attribute = str;
					ungetc(c,stdin);
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
				break;
			case (NUM_POINT):
				if (isdigit(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_FLOAT;
				} else {
					ungetc(c,stdin);
					free(str);
					s_destroy(&s);
					return LEX_ERROR;
				}
				break;
			case (NUM_FLOAT):
				if (isdigit(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_FLOAT;
				} else if (c == 'e' || c == 'E'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_EXP;
				} else {
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_FLOAT;
					token->attribute = str;
					ungetc(c,stdin);
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
				break;
			case (NUM_EXP):
				if (isdigit(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_EXP_FIN;
				} else if (c == '-' || c == '+'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_EXP_OPT;
				} else {
					ungetc(c,stdin);
					free(str);
					s_destroy(&s);
					return LEX_ERROR;
				}
				break;
			case (NUM_EXP_OPT):
				if (isdigit(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_EXP_FIN;
				} else {
					ungetc(c,stdin);
					free(str);
					s_destroy(&s);
					return LEX_ERROR;
				}
				break;
			case (NUM_EXP_FIN):
				if (isdigit(c)){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = NUM_EXP_FIN;
				} else {
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_FLOAT;
					token->attribute = str;
					ungetc(c,stdin);
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
				break;
			case (LINE_COMMENT):
				if (c == '\n'){
					ungetc(c,stdin);
					comment_eol = 1;
					state = START;
				} else if (c == EOF){
					ungetc(c,stdin);
					state = START;
				} else {
					state = LINE_COMMENT;
				}
				break;
			case (BLOCK_COMMENT0):
				if (c == '"'){
					state = BLOCK_COMMENT1;
				} else {
					ungetc(c,stdin);
					free(str);
					s_destroy(&s);
					return LEX_ERROR;
				}
				break;
			case (BLOCK_COMMENT1):
				if (c == '"'){
					state = BLOCK_COMMENT2;
				} else {
					ungetc(c,stdin);
					free(str);
					s_destroy(&s);
					return LEX_ERROR;
				}
				break;
			case (BLOCK_COMMENT2):
				if (c == '"'){
					state = BLOCK_COMMENT3;
					block_end++;
				} else {
					if (!first_token){
						if (!append_char(&str, &str_i, &cap, c)){
							return internal_error_exit(&s, str);
						}
					}
					block_end = 0;
					state = BLOCK_COMMENT2;
				}
				break;
			case (BLOCK_COMMENT3):
				if (c == '"'){
					state = BLOCK_COMMENT4;
					block_end++;
				} else {
					if (!first_token){
						if (!append_char(&str, &str_i, &cap, c)){
							return internal_error_exit(&s, str);
						}
						for (int i = 0; i < block_end; i++){
							if (!append_char(&str, &str_i, &cap, '"')){
								return internal_error_exit(&s, str);
							}
						}
					}
					block_end = 0;
					state = BLOCK_COMMENT2;
				}
				break;
			case (BLOCK_COMMENT4):
				if (c == '"'){
					state = BLOCK_COMMENT4;
					block_end++;
				} else {
					ungetc(c,stdin);
					if (block_end >= 3){
						// dokumentacny retazec - koment
						if (first_token){
							state = START;
							//str_i = 0;
							// zahodenie ulozeneho komentu
							/**
							free(str);
							str = malloc(sizeof(char) * DEFAULT_STR_LEN);
							if (str == NULL){
								// chyba alokacie pamate
								s_destroy(&s);
								return INTERNAL_ERROR;
							}
							**/
						// dokumentacny retazec - string
						} else {
							for (int i = 0; i < block_end -3; i++){
								if (!append_char(&str, &str_i, &cap, '"')){
									return internal_error_exit(&s, str);
								}
							}
							if (!append_char(&str, &str_i, &cap, '\0')){
								return internal_error_exit(&s, str);
							}
							token->type = TK_STRING;
							token->attribute = str;
							if (preload){
								return save_preload(token, &preloaded_token, OK, &preloaded_return);
							} else {
								return OK;
							}	
						}
					} else {
						if (!first_token){
							for (int i = 0; i < block_end; i++){
								if (!append_char(&str, &str_i, &cap, '"')){
									return internal_error_exit(&s, str);
								}
							}
						}
						block_end = 0;
						state = BLOCK_COMMENT2;
					}
				}
				break;
			case (STRING):
				if (c == '\\'){
					state = STRING_ESCSEQ;
				} else if (c == '\''){
					state = STRING_FIN;
				} else if (c >= 32 && c <=127 && c !='\'' && c != '\\'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = STRING;
				} else {
					ungetc(c,stdin);
					free(str);
					s_destroy(&s);
					return LEX_ERROR;
				}
				break;
			case (STRING_ESCSEQ): //otestovat
				if (c >= 32 && c <=127 && c !='x'){
					if (c == '\''){
						if (!append_char(&str, &str_i, &cap, '\'')){
							return internal_error_exit(&s, str);
						}
					} else if (c == '\\'){
						if (!append_char(&str, &str_i, &cap, '\\')){
							return internal_error_exit(&s, str);
						}
					} else if (c == 'n'){
						if (!append_char(&str, &str_i, &cap, '\n')){
							return internal_error_exit(&s, str);
						}
					} else if (c == 't'){
						if (!append_char(&str, &str_i, &cap, '\t')){
							return internal_error_exit(&s, str);
						}
					} else if (c == '\"'){
						if (!append_char(&str, &str_i, &cap, '\"')){
							return internal_error_exit(&s, str);
						}
					} else {
						if (!append_char(&str, &str_i, &cap, '\\')){
							return internal_error_exit(&s, str);
						}
						if (!append_char(&str, &str_i, &cap, c)){
							return internal_error_exit(&s, str);
						}
					}
					state = STRING;
				} else if (c == 'x'){
					state = STRING_HEX0;
				} else {
					ungetc(c,stdin);
					free(str);
					s_destroy(&s);
					return LEX_ERROR;
				}
				break;
			case (STRING_HEX0):
				if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || isalpha(c)){
					hex[0] = c;
					state = STRING_HEX1;
				} else if (c == '\\'){
					if (!append_char(&str, &str_i, &cap, '\\')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, 'x')){
						return internal_error_exit(&s, str);
					}
					state = STRING_ESCSEQ;
				} else if (c == '\''){
					if (!append_char(&str, &str_i, &cap, '\\')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, 'x')){
						return internal_error_exit(&s, str);
					}
					state = STRING_FIN;
				} else {
					if (!append_char(&str, &str_i, &cap, '\\')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, 'x')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = STRING;
				}
				break;
			case (STRING_HEX1):
				if(c == '\''){
					if (!append_char(&str, &str_i, &cap, '\\')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, 'x')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = STRING_FIN;
				} else if (c == '\\'){
					if (!append_char(&str, &str_i, &cap, '\\')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, 'x')){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					state = STRING_ESCSEQ;
				} else {
					if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || isalpha(c)){
						hex[1] = c;
						c = (int)strtol(hex, NULL, 16); 
						if (!append_char(&str, &str_i, &cap, c)){
							return internal_error_exit(&s, str);
						}
					} else {
						if (!append_char(&str, &str_i, &cap, '\\')){
							return internal_error_exit(&s, str);
						}
						if (!append_char(&str, &str_i, &cap, 'x')){
							return internal_error_exit(&s, str);
						}
						if (!append_char(&str, &str_i, &cap, c)){
							return internal_error_exit(&s, str);
						}
					}
					state = STRING;
				}
				break;
			case (STRING_FIN):
				if (!append_char(&str, &str_i, &cap, '\0')){
					return internal_error_exit(&s, str);
				}
				token->type = TK_STRING;
				token->attribute = str;
				ungetc(c,stdin);
				if (preload){
					return save_preload(token, &preloaded_token, OK, &preloaded_return);
				} else {
					return OK;
				}
			case (DIV):
				if (c == '/'){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					token->type = TK_DIV_DIV;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else {
					ungetc(c,stdin);
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_DIV;
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
			case (ASSIGN):
				if (c == '='){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					token->type = TK_EQUAL;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else {
					ungetc(c,stdin);
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_ASSIGN;
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
			case (NEG):
				if (c == '='){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					token->type = TK_NOT_EQUAL;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else {
					ungetc(c,stdin);
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->attribute = str;
					token->type = TK_NEG;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
			case (LESSER):
				if (c == '='){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_LESSER_EQUAL;
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else {
					ungetc(c,stdin);
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_LESSER;
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
			case (GREATER):
				if (c == '='){
					if (!append_char(&str, &str_i, &cap, c)){
						return internal_error_exit(&s, str);
					}
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_GREATER_EQUAL;
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				} else {
					ungetc(c,stdin);
					if (!append_char(&str, &str_i, &cap, '\0')){
						return internal_error_exit(&s, str);
					}
					token->type = TK_GREATER;
					token->attribute = str;
					if (preload){
						return save_preload(token, &preloaded_token, OK, &preloaded_return);
					} else {
						return OK;
					}
				}
			case (INDENTATION):
				// detent flag raised
				if (dedent_flag){
					ungetc(c,stdin);
					if (spc_counter == s_top(&s)){
						spc_counter = 0;
						state = START;
						dedent_flag = 0;
						break;
					} else if (s_top(&s) == 0){
						// zla uroven odsadenia
						free(str);
						s_destroy(&s);
						return LEX_ERROR;
					} else {
						if (!append_char(&str, &str_i, &cap, '\0')){
							return internal_error_exit(&s, str);
						}
						token->type = TK_DEDENT;
						token->attribute = str;
						s_pop(&s);
						if (preload){
							return save_preload(token, &preloaded_token, OK, &preloaded_return);
						} else {
							return OK;
						}
					}
				}
				// normal indentation processing
				if (c == ' '){
					spc_counter++;
					state = INDENTATION;
				} else if (c == '\n' || c == '#' || c == '"'){
					ungetc(c,stdin);
					state = START;
					spc_counter = 0;
				} else {
					ungetc(c,stdin);
					if (spc_counter > s_top(&s)){
						if (!append_char(&str, &str_i, &cap, '\0')){
							return internal_error_exit(&s, str);
						}
						token->type = TK_INDENT;
						token->attribute = str;
						s_push(&s, spc_counter);
						spc_counter = 0;
						if (preload){
							return save_preload(token, &preloaded_token, OK, &preloaded_return);
						} else {
							return OK;
						}
					} else if (spc_counter < s_top(&s)){
						if (!append_char(&str, &str_i, &cap, '\0')){
							return internal_error_exit(&s, str);
						}
						token->type = TK_DEDENT;
						token->attribute = str;
						s_pop(&s);
						if (spc_counter != s_top(&s)){
							dedent_flag = 1;
						}
						if (preload){
							return save_preload(token, &preloaded_token, OK, &preloaded_return);
						} else {
							return OK;
						}
					} else {
						state = START;
					}
				}
			default:
				break;
		}
	}
}