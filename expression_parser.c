/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */


#include "expression_parser.h"
#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include "expression_list.h"

const char precedenceTable[tableSize][tableSize] = {
  // 0  , 1 ,  2,   3,   4,   5.   6,   7,   8,   9,  10,  11,  12,  13,   14,  15,   16, 17, 18
  // +    -    *    /    //   <    <=   >   >=   ==   !=   (     )   id   int  float str none   $
  { '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '#', '>'}, // +       0
  { '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // -       1 
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // *       2
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // /       3
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // //      4
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '<'}, // <       5
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '<'}, // <=      6 
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '<'}, // >       7
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '<'}, // >=      8
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<', '<'}, // ==      9
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<', '<'}, // !=     10
  { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '#'}, // (      11
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // )      12
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // id     13 
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // int    14
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // float  15
  { '>', '#', '#', '#', '#', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // string 16 
  { '#', '#', '#', '#', '#', '#', '#', '#', '#', '>', '>', '#', '#', '>', '#', '#', '#', '#',' >'}, // none   17
  { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '#', '<', '<', '<', '<', '<', '#'}, // $      18
};

int getIndex(Token *token)
{
  // zisti error
  if (token == NULL)
   {
     return -1;
   }

  isRelational = false;

  switch (token->type)
  {
    case (TK_ID): 
          return 13;
          break;
    case (TK_PLUS):
          return 0;
          break;
    case (TK_MINUS):
          return 1;
          break;
    case (TK_MULT):
          return 2; 
          break;
    case (TK_DIV):
          return 3;
          break;
    case (TK_DIV_DIV):
          return 4;
          break; 
    case (TK_EQUAL):
          return 9;
          break;
    case (TK_NOT_EQUAL):
          return 10;
          break;
    case (TK_LESSER):
          return 5;
          break; 
    case (TK_LESSER_EQUAL):
          return 6;
          break;
    case (TK_GREATER):
          return 7;
          break;
    case (TK_GREATER_EQUAL):
          return 8;
          break;
    case (TK_BRACKET_L):
          return 11;
          break;
    case (TK_BRACKET_R):
          return 12;
          break;
    case (TK_INT):
          return 14;
          break;
    case (TK_FLOAT):
          return 15;
          break;
    case (TK_STRING):
          return 16;
          break; 
    case (TK_KW): 
          if ((strcmp(token->attribute, "None")) == 0)
          { 
            return 17;
            break;
          }
          return -2;
          break;
    case (TK_EOL):
          return 18;
          break; 
    default: 
          return -2;     
  }
}


// TODO what if token was already next token? 
int checkDivisionByZero(Token *token)
{
  Token *nextToken;
  get_next_token(nextToken,1);

  if (!strcmp(nextToken -> attribute, "0"))
  {
    return SYNTAX_ERROR;
  }

}

int callExpression(Token *token)
{
  // so far, we don't know if the operand is relational 
  isRelational = false;
  int leftBracket = 0;
  int rightBracket =0; 
  exprList *eList;
  listInsertFirst(eList,token);

  // Load tokens into list 
  do 
  {
      token=get_next_token(token,0); 
      //count number of brackets - must be even
      if (token->type == TK_BRACKET_L)
      {
            leftBracket+=1;
      }

      if (token->type == TK_BRACKET_R)
      {
            rightBracket+=1;
      }
      
      if (token->type == TK_EQUAL)
      {
            isRelational = true; 
      }
      else if (token->type == TK_NOT_EQUAL)
      {
            isRelational =true; 
      }
      else if (token->type == TK_LESSER)
      {
            isRelational = true;
      }
      else if (token->type == TK_LESSER_EQUAL)
      {
            isRelational = true;
      }
      else if ( token->type == TK_GREATER)
      {
            isRelational = true; 
      }
      else if (token->type == TK_GREATER_EQUAL)
      {
            isRelational = true;
      }

      listInsertAct(eList,token);
      
  }
  while ( token->attribute != TK_EOL);

  if ( leftBracket != rightBracket) 
  {
      fprintf(stderr, "Number of left brackets doesnt match number of right brackets");
      return SYNTAX_ERROR;
  }


  // index to the table  
  int indexStack=getIndex(token);
  if ( indexStack == -1)
  {
    return INTERNAL_ERROR;
  }
  if ( indexStack == -2)
  {
    return SYNTAX_ERROR;
  }
  // TODO save token the stack

  // index of another token
  token = get_next_token(token,0);
  int indexInput=getIndex(token);
  if ( indexInput == -1)
  {
    return INTERNAL_ERROR;
  }
  if ( indexInput == -2)
  {
    return SYNTAX_ERROR;
  }
  
  // check the symbol in precedence table 
  char symbol = precedenceTable[indexStack][indexInput];
  // # not defined sequence of operands and operators in expression, so syntax error
  if ( (strcmp(symbol, "#") )== 0)
  {
    return SYNTAX_ERROR;
  }


  // after while, when all the tokens are loaded to stack 
  // check if the brackets are even and there is the same number 
  // of left brackets and right brackets 

  // TODO get the rule and push it to the stack
  // TODO continue with other tokens 
}