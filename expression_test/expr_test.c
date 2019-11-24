/** 
 * 
 * Testing expression parser
 * 
 */

#include <stdio.h>
#include "expression_parser.h"

Token* testToken; 

int main()
{
    printf("FIRST TOKEN \n");
    printf("token: 12 \n");
    printf("token type is TK_INT, so number 22 \n"); 
    printf("token is data type TYPE_INT so number 0 \n");
    printf("\n \n");

    printf("SECOND TOKEN \n");
    printf("token: + \n");
    printf("token type is TK_PLUS, so number 7 \n"); 
    printf("token is data type TYPE_UNDEFINED so number 4 \n");
    printf("\n \n");

    printf("THIRD TOKEN \n");
    printf("token: 42.12 \n");
    printf("token type is TK_FLOAT, so number 23 \n"); 
    printf("token is data type TYPE_FLOAT so number 1 \n");
    printf("\n \n");

    printf("FOURTH TOKEN \n");
    printf("token: * \n");
    printf("token type is TK_MULT, so number 9 \n"); 
    printf("token is data type TYPE_UNDEFINED so number 4 \n");
    printf("\n \n");

    printf("FIFTH TOKEN \n");
    printf("token: 10 \n");
    printf("token type is TK_INT, so number 22 \n"); 
    printf("token is data type TYPE_UNDEFINED so number 0 \n");
    printf("\n \n");

    printf("SIXTH TOKEN \n");
    printf("token: != \n");
    printf("token type is TK_NOTEQ, so number 13 \n"); 
    printf("token is data type TYPE_UNDEFINED so number 4 \n");
    printf("\n \n");

    printf("SEVENT TOKEN \n");
    printf("token: None \n");
    printf("token type is TK_KW, so number 3 \n"); 
    printf("token is data type TYPE_NONE so number 3 \n");
    printf("\n \n");

    get_next_token(testToken);

    while ( testToken->type != EOF)
    {
        printf("Token is of type:   %d \n", testToken->type);
        printf("Token is of type in PTABLE:   %d\n", getIndex(testToken));
        printf("Token is of data type:   %d \n", getDataType(testToken));
        printf("\n \n *********************************** \n \n");
        get_next_token(testToken);
    }

    // callExpression(testToken);

    return 0; 
}
