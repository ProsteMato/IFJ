#include "./parser.h"

#define GET_NEXT_TOKEN(token) if (get_next_token(0, token) == LEX_ERROR) return LEX_ERROR

int main(int argc, char const *argv[])
{
    Token token;
    GET_NEXT_TOKEN(&token);
    printf("Return code: %d", prog(&token));
    return 0;
}
