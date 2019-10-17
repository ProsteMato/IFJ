/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief Scanner implementation.
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "scanner.h"


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
	token->type = TK_EMPTY;

	while(TRUE){
		c = getc(source);
		switch(state){
			case (START):
				if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z'){
					state = ID_OR_KW;
				} else if (isdigit(c)){
					state = NUM;
				} else if (isspace(c)){
					stare = START;
				} else if (c == '_'){
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
					return ;
				}
				break;
			case (EOL):
				break;
			case (ID_OR_KW):
				break;
			case (NUM):
				break;
			default:
				break;
		}
	}
}