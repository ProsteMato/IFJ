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
	} else if (strncmp (s, "int", len) == 0){
		return KW_INT;
	} else if (strncmp (s, "str", len) == 0){
		return KW_STR;
	} else if (strncmp (s, "float", len) == 0){
		return KW_FLOAT;
	} else {
		return -1;
	}
}

int get_next_token(FILE *source, Token *token){
	// kontrola source filu
	if (source == NULL){
		return 1;
	} else if (token == NULL){
		return 1;
	}

	// inicializacia premennych
	char* str = malloc(DEFAULT_STR_LEN);
	unsigned long str_i = 0;
	char c;
	int state = START;
	int keyword = 0;
	token->type = TK_EMPTY;

	while(TRUE){
		c = getc(source);
		switch(state){
			case (START):
				if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z'){
					str[str_i] = c;
					str_i++;
					state = ID_OR_KW;
				} else if (isdigit(c)){
					str[str_i] = c;
					str_i++;
					state = NUM;
				} else if (isspace(c)){
					stare = START;
				} else if (c == '_'){
					str[str_i] = c;
					str_i++;
					state = ID;					
				} else if (c == '#'){
					state = LINE_COMMENT;				
				} else if (c == '"'){
					state = BLOCK_COMMENT0;						
				} else if (c == 39){ // ascii 39 == '
					state = BLOCK_COMMENT0;
				} else if (c == '/'){
					state = DIV;										
				} else if (c == '='){
					state = ASSIGN;
				} else if (c == '<'){
					state = LESSER;
				} else if (c == '>'){
					state = GREATER;
				} else if (c == '!'){
					state = NEG;
				} else if (c == '\n'){
					state = EOL;
				} else if (c == '*'){
					token->type = MULT;
					free(str);
					return 0;
				} else if (c == '+'){
					token->type = PLUS;
					free(str);
					return 0;
				} else if (c == '-'){
					token->type = MINUS;
					free(str);
					return 0;
				} else if (c == ':'){
					token->type = COLON;
					free(str);
					return 0;
				} else if (c == '('){
					token->type = BRACKET_L;
					free(str);
					return 0;
				} else if (c == ')'){
					token->type = BRACKET_R;
					free(str);
					return 0;
				} else if (c == ','){
					token->type = COMMA;
					free(str);
					return 0;
				} else {
					// lex chyba
					free(str);
					return 1;
				}
				break;
			case (ID):
				if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || isdigit(c) || c == '_'){
					str[str_i] = c;
					str_i++;
					state = ID;
				} else {
					token->type = TK_ID;
					token->atribute.string = str;
					ungetc(c, source);
					return 0;
				}
				break;
			case (ID_OR_KW):
				if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z'){
					str[str_i] = c;
					str_i++;
					keyword = is_keyword(str, str_i);
					state = ID_OR_KW;
				} else if (isdigit(c) || c == '_'){
					str[str_i] = c;
					str_i++;
					state = ID;
				} else {
					if (keyword >= 0){
						token->type = TK_KW;
						token->atribute.keyword = keyword;
						ungetc(c, source);
						free(str);
						return 0;
					} else {
						token->type = TK_ID;
						toke->atribute.string = str;
						ungetc(c, source);
						return 0;
					}
				}
				break;
			case (NUM):
				if (isdigit(c){
					str[str_i] = c;
					str_i++;
					state = NUM;
				} else if (isdigit(c) || c == '_'){
					str[str_i] = c;
					state = ID;
				} else {
					token->type = TK_INT;
					token->atribute.integer = strol(str,NULL,10);
					ungetc(c,source);
					free(str);
					return 0;
				}
				break;
			default:
				break;
		}
	}
}