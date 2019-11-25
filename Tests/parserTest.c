#include "../parser.h"
#include "../scanner.h"

#define GET_NEXT_TOKEN(token) \
		if ((token_return_value = get_next_token((token))) != OK) \
			return token_return_value

int main(int argc, char const *argv[])
{
    tkq_init();
    Token token;
    int token_return_value = 0;
    
    GET_NEXT_TOKEN(&token);

    //int i = 0;
    // while(token.type != TK_EOF) {
    // 	printf("token %d: type: %d attribute: %s \n", i, token.type, token.attribute);
    // 	if (get_next_token(&token, 0) == LEX_ERROR){
    // 		printf("%d\n", token.type);
    // 		return LEX_ERROR;
    // 	}
    // 	i++;
    // }
    
    int returnType = prog(&token);
    printf("Return code: %d\n",  returnType);

    return 0;
}
