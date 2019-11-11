#include <stdlib.h>
#include "scanner.h"

int main (){
	Token token;
	int i;
	for (int e = 1; e < 600; e++){
		i = get_next_token(&token);
		if (i == LEX_ERROR || i == INTERNAL_ERROR || token.type == TK_EOF){
			if (token.type == TK_EOF){
				printf("EOF\n");
			} else if (i == LEX_ERROR){
				printf("LEX_ERROR\n");
			} else {
				printf("INTERNAL_ERROR\n");
			}
			printf("koniec japonec\n");
			break;
		}
		printf("+++++%d return: %d token: ", e, i);
		switch (token.type){
			case(TK_EOF):
				printf("TK_EOF\n");
				break;
			case(TK_EOL):
				printf("TK_EOL\n");
				break;
			case(TK_ID):
				printf("TK_ID\n");
				break;
			case(TK_KW):
				printf("TK_KW\n");
				break;
			case(TK_COMMA):
				printf("TK_COMMA\n");
				break;
			case(TK_BRACKET_L):
				printf("TK_BRACKET_L\n");
				break;
			case(TK_BRACKET_R):
				printf("TK_BRACKET_R\n");
				break;
			case(TK_PLUS):
				printf("TK_PLUS\n");
				break;
			case(TK_MINUS):
				printf("TK_MINUS\n");
				break;
			case(TK_MULT):
				printf("TK_MULT\n");
				break;
			case(TK_DIV):
				printf("TK_DIV\n");
				break;
			case(TK_DIV_DIV):
				printf("TK_DIV_DIV\n");
				break;
			case(TK_EQUAL):
				printf("TK_EQUAL\n");
				break;
			case(TK_NOT_EQUAL):
				printf("TK_NOT_EQUAL\n");
				break;
			case(TK_NEG):
				printf("TK_NEG\n");
				break;
			case(TK_ASSIGN):
				printf("TK_ASSIGN\n");
				break;
			case(TK_LESSER):
				printf("TK_LESSER\n");
				break;
			case(TK_LESSER_EQUAL):
				printf("TK_LESSER_EQUAL\n");
				break;
			case(TK_GREATER):
				printf("TK_GREATER\n");
				break;
			case(TK_GREATER_EQUAL):
				printf("TK_GREATER_EQUAL\n");
				break;
			case(TK_COLON):
				printf("TK_COLON\n");
				break;
			case(TK_STRING):
				printf("TK_STRING\n");
				break;
			case(TK_INT):
				printf("TK_INT\n");
				break;
			case(TK_FLOAT):
				printf("TK_FLOAT\n");
				break;
			case(TK_EMPTY):
				printf("TK_EMPTY\n");
				break;
			case(TK_INDENT):
				printf("TK_INDENT\n");
				break;
			case(TK_DEDENT):
				printf("TK_DEDENT\n");
				break;
			default:
				printf("CHYBAAAAAAA\n");
				break;
		}
	}
}