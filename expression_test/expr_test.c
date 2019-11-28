/** 
 * 
 * Testing expression parser
 * 
 */

#include <stdlib.h>
#include "../expression_parser.h"
#include "../scanner.h"

Token testToken; 
int error;

int main()
{
    int e = get_next_token(&testToken);
    if (e != OK)
    {
        printf("Error: %d \n", e);
    }
    int result = callExpression(&testToken);
    printf("Result of callExpression %d \n", result);
    for (int i=0; i < 15; i++)
    {
          printf("Rules used were: %d \n", precedenceRules[i]);
    }
    if ( isRelational )
    {
          printf("There was a relational operand.\n");
    }
    else  printf("There wasnt a relational operand.\n");
    printf("BLABLABLABLALBALABLALa\n");
    /**
    void tkq_init(); 
  int leftBracket = 0;
  int rightBracket =0; 
  int e = get_next_token(&testToken);
  if ( e != OK )
  {
      printf("Error with getting next token: %d\n", e);
  }
  pTable symbol= getIndex(&testToken);
  exprList eList;
  data_type dType= getDataType(&testToken);
  if (symbol == -1 )
  {
      error = INTERNAL_ERROR;
      return error;
  }
  listInitialize(&eList);
  listInsertFirst(&eList,symbol,dType);
  
  // Load tokens into list 
  for (int i =0; i<7; i++)
  {
      e = get_next_token(&testToken); 
      if  (e != OK)
      {     
            error= e;
            return error;
      }
       printf("%d\n", i);
        printf("token is: %s \n", testToken.attribute);
        printf("Token is of type:   %d \n", testToken.type);
      symbol= getIndex(&testToken);
      if (symbol == -1 )
      {
            listDispose(&eList);
            error= INTERNAL_ERROR;
            return error;
      }
      data_type dType= getDataType(&testToken);
      if (testToken.type == TK_KW)
      {
            if ((strcmp(testToken.attribute, "None")) != 0)
            { 
                  listDispose(&eList);
                  error = SYNTAX_ERROR;
                  return error;
            }
      }
      //count number of brackets - must be even
      if (testToken.type == TK_BRACKET_L)
      {
            leftBracket+=1;
      }

      if (testToken.type == TK_BRACKET_R)
      {
            rightBracket+=1;
      }
      
      if (testToken.type == TK_EQUAL)
      {
            isRelational = true; 
      }
      else if (testToken.type == TK_NOT_EQUAL)
      {
            isRelational =true; 
      }
      else if (testToken.type == TK_LESSER)
      {
            isRelational = true;
      }
      else if (testToken.type == TK_LESSER_EQUAL)
      {
            isRelational = true;
      }
      else if ( testToken.type == TK_GREATER)
      {
            isRelational = true; 
      }
      else if (testToken.type == TK_GREATER_EQUAL)
      {
            isRelational = true;
      }

      if (testToken.type == TK_DIV)
      {
            int div = checkDivisionByZero(&testToken);
            
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(&eList);
                  fprintf(stderr, "Div with zero.");
                  error= DIVISION_BY_ZERO_ERROR;
                  return error;
            }
      } 
      
      if (testToken.type == TK_DIV_DIV)
      {
            int div = checkDivisionByZero(&testToken);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(&eList);
                  fprintf(stderr, "Div Div with zero.");
                  error = SYNTAX_ERROR;
                  return error;
            }
      }
      listInsertAct(&eList,symbol, dType);
      printf("Token v liste je: %d \n" , eList.act->symbol);
  }
    printf("Number of left brackets is: %d\n",leftBracket);
    printf("Number of right brackets is: %d\n", rightBracket);
    if ( isRelational)
    {
        printf("There was a relational operator. \n");
    }
    else printf( "There wasnt a relational operator. \n");
  if ( leftBracket != rightBracket) 
  {
      fprintf(stderr, "Number of left brackets doesnt match number of right brackets");
      listDispose(&eList);
      error = SYNTAX_ERROR;
      return error;
  } */
    /*
    void tkq_init(); 
    exprList eList;
    listInitialize(&eList);
    printf("List initialized. \n");
    int e= get_next_(token)(&testToken);
    if ( e != OK)
    {
        printf("Error: %d\n ", e);
        return e;
    }
    int l = listInsertFirst(&eList,getIndex(&testToken),getDataType(&testToken));
    if ( l != OK)
    {
        printf("Error with list %d\n", l);
        return l;
    }
    else  printf("First inserted succesfully.\n");
    for (int i=0; i < 6; i++)
    {
        e= get_next_token(&testToken);
        if ( e != OK)
        {
            printf("Error: %d\n ", e);
            return e;
        }
        printf("%d\n", i);
        printf("token is: %s \n", testToken.attribute);
        printf("Token is of type:   %d \n", testToken.type);
        printf("Token is of type in PTABLE:   %d\n", getIndex(&testToken));
        printf("Token is of data type:   %d \n", getDataType(&testToken));
        l = listInsertAct(&eList, getIndex(&testToken), getDataType(&testToken));
        if ( l != OK)
        {
            printf("Error with list %d\n", l);
            return l;
        }
        else  printf("Token inserted succesfully.\n");
        printf("\n \n*********************************** \n \n");
    }
    
    exprList* eList = NULL;
    listInitialize(eList);
    eList=createList(&testToken);
    if ( error == SYNTAX_ERROR)
    {
      listDispose(eList);
      return SYNTAX_ERROR;
    }

   if ( error == INTERNAL_ERROR)
    {
      listDispose(eList);
      return INTERNAL_ERROR;
    }
    */
    return OK; 
}
