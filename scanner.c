/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief Scanner implementation.
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "scanner.h"

int append_char (char *str, unsigned long *i, unsigned long *cap, char c){
	if (*i >= *cap -1){
		*cap += DEFAULT_STR_LEN;
		char *tmp = realloc (str, sizeof(char) * cap);
		if (tmp == NULL){
			// chyba alokacie pamate
			return -1;
		} else {
			str = tmp;
		}
	}
	str[*i] = c;
	*i++;
	return 0;
}

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
	char* str = malloc(sizeof(char) * DEFAULT_STR_LEN);
	if (str == NULL){
		// chyba alokacie pamate
		return 1;
	}
	unsigned long cap = DEFAULT_STR_LEN;
	unsigned long str_i = 0;
	char c;
	int state = START;
	int keyword = 0;
	token->type = TK_EMPTY;

	while(TRUE){
		c = getc(source);
		switch(state){
			case (START):
				if (isalpha(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = ID_OR_KW;
				} else if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM;
				} else if (isspace(c)){
					stare = START;
				} else if (c == '_'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
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
				if (isalpha(c) || isdigit(c) || c == '_'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = ID;
				} else {
					token->type = TK_ID;
					token->atribute.string = str;
					ungetc(c, source);
					return 0;
				}
				break;
			case (ID_OR_KW):
				if (isalpha(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					keyword = is_keyword(str, str_i);
					state = ID_OR_KW;
				} else if (isdigit(c) || c == '_'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
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
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM;
				} else if (c == '.'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_POINT;
				} else if (c == 'E' || c == 'e'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_EXP;
				} else {
					token->type = TK_INT;
					token->atribute.integer = atoi(str);
					ungetc(c,source);
					free(str);
					return 0;
				}
				break;
			case (NUM_POINT):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_FLOAT;
				} else {
					ungetc(c,source);
					free(str);
					return 1;
				}
				break;
			case (NUM_FLOAT):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_FLOAT;
				} else if (c == 'e' || c == 'E'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_EXP;
				} else {
					token->type = TK_FLOAT;
					token->atribute.decimal = strtod(str,NULL);
					ungetc(c,source);
					free(str);
					return 0;
				}
				break;
			case (NUM_EXP):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_EXP_FIN;
				} else if (c == '-' || c == '+'){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_EXP_OPT;
				} else {
					ungetc(c,source);
					free(str);
					return 1;
				}
				break;
			case (NUM_EXP_OPT):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_EXP_FIN;
				} else {
					ungetc(c,source);
					free(str);
					return 1;
				}
				break;
			case (NUM_EXP_FIN):
				if (isdigit(c)){
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = NUM_EXP_FIN;
				} else {
					token->type = TK_FLOAT;
					token->atribute.decimal = strtod(str,NULL);
					ungetc(c,source);
					free(str);
					return 0;
				}
				break;
			case (LINE_COMMENT):
				if (c == '\n'){
					ungetc(c,source);
					state = START;
				} else {
					state = LINE_COMMENT;
				}
			case (BLOCK_COMMENT0):
				if (c == '"'){
					state = BLOCK_COMMENT1;
				} else {
					ungetc(c,source);
					free(str);
					return 1;
				}
				break;
			case (BLOCK_COMMENT1):
				if (c == '"'){
					state = BLOCK_COMMENT2;
				} else {
					ungetc(c,source);
					free(str);
					return 1;
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
			case (BLOCK_COMMENT04):
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
						return -1;
					}
					state = STRING;
				} else {
					ungetc(c,source);
					free(str);
					return 1;
				}
				break;
			case (STRING_ESCSEQ): //otestovat
				if (c >= 32 && c <=127 && c !='x'){
					if (c == '\''){
						if (!append_char(str, &str_i, &cap, '\'')){
							free(str);
							return -1;
						}
					} else if (c == '\\'){
						if (!append_char(str, &str_i, &cap, '\\')){
							free(str);
							return -1;
						}
					} else if (c == 'n'){
						if (!append_char(str, &str_i, &cap, '\n')){
							free(str);
							return -1;
						}
					} else if (c == 't'){
						if (!append_char(str, &str_i, &cap, '\t')){
							free(str);
							return -1;
						}
					} else if (c == '\"'){
						if (!append_char(str, &str_i, &cap, '\"')){
							free(str);
							return -1;
						}
					} else {
						if (!append_char(str, &str_i, &cap, '\\')){
							free(str);
							return -1;
						}
						if (!append_char(str, &str_i, &cap, c)){
							free(str);
							return -1;
						}
					}
					state = STRING;
				} else if (c == 'x'){
					state = STRING_HEX0;
				} else {
					ungetc(c,source);
					free(str);
					return 1;
				}
				break;
			case (STRING_HEX0):
				if (c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F' || isalpha(c)){
					char hex[2];
					hex[0] = c;
					state = STRING_HEX1;
				} else if (c == '\\'){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return -1;
					}
					state = STRING_ESCSEQ;
				} else if (c == '\''){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return -1;
					}
					state = STRING_FIN;
				} else {
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = STRING;
				}
				break;
			case (STRING_HEX1):
				if(c == '\''){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = STRING_FIN;
				} else if (c == '\\'){
					if (!append_char(str, &str_i, &cap, '\\')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, 'x')){
						free(str);
						return -1;
					}
					if (!append_char(str, &str_i, &cap, c)){
						free(str);
						return -1;
					}
					state = STRING_ESCSEQ;
				} else {
					if (c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F' || isalpha(c)){
						hex[1] = c;
						c = (int)strtol(hex, NULL, 16); 
						if (!append_char(str, &str_i, &cap, c)){
							free(str);
							return -1;
						}
					} else {
						if (!append_char(str, &str_i, &cap, '\\')){
							free(str);
							return -1;
						}
						if (!append_char(str, &str_i, &cap, 'x')){
							free(str);
							return -1;
						}
						if (!append_char(str, &str_i, &cap, c)){
							free(str);
							return -1;
						}
					}
					state = STRING;
				}
				break;
			case (STRING_FIN):
				str[str_i] = '\0';
				token->type = TK_STRING;
				token->atribute.string = str;
				ungetc(c,source);
				return 0;
				break;
			case (EOL):
				if (c == '#'){
					ungetc(c,source);
					state = START;
				} else if (c == ' '){
					state = EOL;
				} else if (c == '\t'){
					state = EOL;
				} else {
					ungetc(c,source);
					state = START;
				}
				break;
			case (DIV):
				if (c == '/'){
					token->type = TK_DIV_DIV;
					return 0;
				} else {
					ungetc(c,source);
					token->type = TK_DIV;
					return 0;
				}
				break;
			case (ASSIGN):
				if (c == '='){
					token->type = TK_EQUAL;
					return 0;
				} else {
					ungetc(c,source);
					token->type = TK_ASSIGN;
					return 0;
				}
				break;
			case (NEG):
				if (c == '='){
					token->type = TK_NOT_EQUAL;
					return 0;
				} else {
					ungetc(c,source);
					token->type = TK_NEG;
					return 0;
				}
				break;
			case (LESSER):
				if (c == '='){
					token->type = TK_LESSER_EQUAL;
					return 0;
				} else {
					ungetc(c,source);
					token->type = TK_LESSER;
					return 0;
				}
				break;
			case (GREATER):
				if (c == '='){
					token->type = TK_GREATER_EQUAL;
					return 0;
				} else {
					ungetc(c,source);
					token->type = TK_GREATER;
					return 0;
				}
				break;
			default:
				break;
		}
	}
}