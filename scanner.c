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

int get_next_token(Token *token){
	// kontrola parametrov
	if (token == NULL){
		return INTERNAL_ERROR;
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
	char* str = malloc(sizeof(char) * DEFAULT_STR_LEN);
	if (str == NULL){
		// chyba alokacie pamate
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
	token->type = TK_EMPTY;
	if (dedent_flag){
		state = INDENTATION;
	}

	while(true){
		c = getc(stdin);
		//printf("----------------------------\n");
		//printf("state %d char '%c' str_i %ld\n", state, c, str_i);
		//printf("comment_eol %d first_token %d\n", comment_eol, first_token);
		//printf("spc_counter %d s_top %d\n", spc_counter, s_top(&s));
		switch(state){
			case (START):
			str_i = 0;
				if (isalpha(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					};
					first_token = 0;
					state = ID_OR_KW;
				} else if (c == '/'){
					first_token = 0;
					state = DIV;	
				} else if (c == EOF){
					free(str);
					token->type = TK_EOF;
					return OK;								
				} else if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
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
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
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
					state = DIV;										
				} else if (c == '='){
					first_token = 0;
					state = ASSIGN;
				} else if (c == '<'){
					first_token = 0;
					state = LESSER;
				} else if (c == '>'){
					first_token = 0;
					state = GREATER;
				} else if (c == '!'){
					first_token = 0;
					state = NEG;
				} else if (c == '\n'){
					if (comment_eol && !first_token){
						ungetc(c, stdin);
						token->type = TK_EOL;
						spc_counter = 0;
						first_token = 1;
						return OK;
					}
					if (first_token){
						state = INDENTATION;
					} else {
						token->type = TK_EOL;
						spc_counter = 0;
						first_token = 1;
						ungetc(c, stdin);
						free(str);
						return OK;
					}
				} else if (c == '*'){
					first_token = 0;
					token->type = TK_MULT;
					free(str);
					return OK;
				} else if (c == '+'){
					first_token = 0;
					token->type = TK_PLUS;
					free(str);
					return OK;
				} else if (c == '-'){
					first_token = 0;
					token->type = TK_MINUS;
					free(str);
					return OK;
				} else if (c == ':'){
					first_token = 0;
					token->type = TK_COLON;
					free(str);
					return OK;
				} else if (c == '('){
					first_token = 0;
					token->type = TK_BRACKET_L;
					free(str);
					return OK;
				} else if (c == ')'){
					first_token = 0;
					token->type = TK_BRACKET_R;
					free(str);
					return OK;
				} else if (c == ','){
					first_token = 0;
					token->type = TK_COMMA;
					free(str);
					return OK;
				} else {
					// lex chyba
					free(str);
					return LEX_ERROR;
				}
				break;
			case (ID):
				if (isalpha(c) || isdigit(c) || c == '_'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = ID;
				} else {
					token->type = TK_ID;
					token->attribute = str;
					ungetc(c, stdin);
					return OK;
				}
				break;
			case (ID_OR_KW):
				if (isalpha(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					keyword = is_keyword(str, str_i);
					state = ID_OR_KW;
				} else if (isdigit(c) || c == '_'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = ID;
				} else {
					if (keyword >= 0){
						token->type = TK_KW;
						token->attribute = str;
						ungetc(c, stdin);
						free(str);
						return OK;
					} else {
						token->type = TK_ID;
						token->attribute = str;
						ungetc(c, stdin);
						return OK;
					}
				}
				break;
			case (NUM):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM;
				} else if (c == '.'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_POINT;
				} else if (c == 'E' || c == 'e'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_EXP;
				} else {
					token->type = TK_INT;
					token->attribute = str;
					ungetc(c,stdin);
					free(str);
					return OK;
				}
				break;
			case (NUM_POINT):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_FLOAT;
				} else {
					ungetc(c,stdin);
					free(str);
					return LEX_ERROR;
				}
				break;
			case (NUM_FLOAT):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_FLOAT;
				} else if (c == 'e' || c == 'E'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_EXP;
				} else {
					token->type = TK_FLOAT;
					token->attribute = str;
					ungetc(c,stdin);
					free(str);
					return OK;
				}
				break;
			case (NUM_EXP):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_EXP_FIN;
				} else if (c == '-' || c == '+'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_EXP_OPT;
				} else {
					ungetc(c,stdin);
					free(str);
					return LEX_ERROR;
				}
				break;
			case (NUM_EXP_OPT):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_EXP_FIN;
				} else {
					ungetc(c,stdin);
					free(str);
					return LEX_ERROR;
				}
				break;
			case (NUM_EXP_FIN):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = NUM_EXP_FIN;
				} else {
					token->type = TK_FLOAT;
					token->attribute = str;
					ungetc(c,stdin);
					free(str);
					return OK;
				}
				break;
			case (LINE_COMMENT):
				if (c == '\n'){
					ungetc(c,stdin);
					comment_eol = 1;
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
					return LEX_ERROR;
				}
				break;
			case (BLOCK_COMMENT1):
				if (c == '"'){
					state = BLOCK_COMMENT2;
				} else {
					ungetc(c,stdin);
					free(str);
					return LEX_ERROR;
				}
				break;
			case (BLOCK_COMMENT2):
				if (c == '"'){
					state = BLOCK_COMMENT3;
				} else {
					state = BLOCK_COMMENT2;
				}
				break;
			case (BLOCK_COMMENT3):
				if (c == '"'){
					state = BLOCK_COMMENT4;
				} else {
					state = BLOCK_COMMENT2;
				}
			case (BLOCK_COMMENT4):
				if (c == '"'){
					state = START;
				} else {
					state = BLOCK_COMMENT2;
				}
				break;
			case (STRING):
				if (c == '\\'){
					state = STRING_ESCSEQ;
				} else if (c == '\''){
					state = STRING_FIN;
				} else if (c >= 32 && c <=127 && c !='\'' && c != '\\'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = STRING;
				} else {
					ungetc(c,stdin);
					free(str);
					return LEX_ERROR;
				}
				break;
			case (STRING_ESCSEQ): //otestovat
				if (c >= 32 && c <=127 && c !='x'){
					if (c == '\''){
						if (!append_char(str, &str_i, &cap, '\'')){
							free(str);
							return INTERNAL_ERROR;
						}
					} else if (c == '\\'){
						if (!append_char(str, &str_i, &cap, '\\')){
							free(str);
							return INTERNAL_ERROR;
						}
					} else if (c == 'n'){
						if (!append_char(str, &str_i, &cap, '\n')){
							free(str);
							return INTERNAL_ERROR;
						}
					} else if (c == 't'){
						if (!append_char(str, &str_i, &cap, '\t')){
							free(str);
							return INTERNAL_ERROR;
						}
					} else if (c == '\"'){
						if (!append_char(str, &str_i, &cap, '\"')){
							free(str);
							return INTERNAL_ERROR;
						}
					} else {
						if (!append_char(str, &str_i, &cap, '\\')){
							free(str);
							return INTERNAL_ERROR;
						}
						if (!append_char(str, &str_i, &cap, c)){
							free(str);
							return INTERNAL_ERROR;
						}
					}
					state = STRING;
				} else if (c == 'x'){
					state = STRING_HEX0;
				} else {
					ungetc(c,stdin);
					free(str);
					return LEX_ERROR;
				}
				break;
			case (STRING_HEX0):
				if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || isalpha(c)){
					hex[0] = c;
					state = STRING_HEX1;
				} else if (c == '\\'){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return INTERNAL_ERROR;
					}
					state = STRING_ESCSEQ;
				} else if (c == '\''){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return INTERNAL_ERROR;
					}
					state = STRING_FIN;
				} else {
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = STRING;
				}
				break;
			case (STRING_HEX1):
				if(c == '\''){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = STRING_FIN;
				} else if (c == '\\'){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return INTERNAL_ERROR;
					}
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					state = STRING_ESCSEQ;
				} else {
					if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || isalpha(c)){
						hex[1] = c;
						c = (int)strtol(hex, NULL, 16); 
						if (!append_char(str, &str_i, &cap, c)){
							free(str);
							return INTERNAL_ERROR;
						}
					} else {
						if (!append_char(str, &str_i, &cap, '\\')){
							free(str);
							return INTERNAL_ERROR;
						}
						if (!append_char(str, &str_i, &cap, 'x')){
							free(str);
							return INTERNAL_ERROR;
						}
						if (!append_char(str, &str_i, &cap, c)){
							free(str);
							return INTERNAL_ERROR;
						}
					}
					state = STRING;
				}
				break;
			case (STRING_FIN):
				str[str_i] = '\0';
				token->type = TK_STRING;
				token->attribute = str;
				ungetc(c,stdin);
				return OK;
				break;
			case (DIV):
				if (c == '/'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					token->attribute = str;
					token->type = TK_DIV_DIV;
					return OK;
				} else {
					ungetc(c,stdin);
					token->type = TK_DIV;
					token->attribute = str;
					return OK;
				}
				break;
			case (ASSIGN):
				if (c == '='){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					token->attribute = str;
					token->type = TK_EQUAL;
					return OK;
				} else {
					ungetc(c,stdin);
					token->type = TK_ASSIGN;
					token->attribute = str;
					return OK;
				}
				break;
			case (NEG):
				if (c == '='){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					token->attribute = str;
					token->type = TK_NOT_EQUAL;
					return OK;
				} else {
					ungetc(c,stdin);
					token->type = TK_NEG;
					return OK;
				}
				break;
			case (LESSER):
				if (c == '='){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					token->type = TK_LESSER_EQUAL;
					token->attribute = str;
					return OK;
				} else {
					ungetc(c,stdin);
					token->type = TK_LESSER;
					token->attribute = str;
					return OK;
				}
				break;
			case (GREATER):
				if (c == '='){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return INTERNAL_ERROR;
					}
					token->type = TK_GREATER_EQUAL;
					token->attribute = str;
					return OK;
				} else {
					ungetc(c,stdin);
					token->type = TK_GREATER;
					token->attribute = str;
					return OK;
				}
				break;
			case (INDENTATION):
				if (dedent_flag){
					ungetc(c,stdin);
					if (spc_counter == s_top(&s)){
						spc_counter = 0;
						state = START;
					} else if (s_top(&s) == -1){
						// zla uroven odsadenia
						free(str);
						return LEX_ERROR;
					} else {
						token->type = TK_DEDENT;
						s_pop(&s);
						return OK;
					}
				}

				if (c == ' '){
					spc_counter++;
					state = INDENTATION;
				} else if (c == '\n' || c == '#'){
					ungetc(c,stdin);
					state = START;
					spc_counter = 0;
				} else {
					ungetc(c,stdin);
					if (spc_counter > s_top(&s)){
						token->type = TK_INDENT;
						s_push(&s, spc_counter);
						spc_counter = 0;
						return OK;
					} else if (spc_counter < s_top(&s)){
						token->type = TK_DEDENT;
						s_pop(&s);
						if (spc_counter != s_top(&s)){
							dedent_flag = 1;
						}
						return OK;
					} else {
						state = START;
					}
				}
				break;
			default:
				break;
		}
	}
}