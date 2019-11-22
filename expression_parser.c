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
#include "symtable.h"

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
          return PT_ID;
          break;
    case (TK_PLUS):
          return PT_PLUS;
          break;
    case (TK_MINUS):
          return PT_MINUS;
          break;
    case (TK_MULT):
          return PT_MULT; 
          break;
    case (TK_DIV):
          return PT_DIV;
          break;
    case (TK_DIV_DIV):
          return PT_DIV_DIV;
          break; 
    case (TK_EQUAL):
          return PT_EQ;
          break;
    case (TK_NOT_EQUAL):
          return PT_NOT_EQ;
          break;
    case (TK_LESSER):
          return PT_LESS;
          break; 
    case (TK_LESSER_EQUAL):
          return PT_LESS_EQ;
          break;
    case (TK_GREATER):
          return PT_GREAT;
          break;
    case (TK_GREATER_EQUAL):
          return PT_GREAT_EQ;
          break;
    case (TK_BRACKET_L):
          return PT_LEFT_BRACK;
          break;
    case (TK_BRACKET_R):
          return PT_RIGHT_BRACK;
          break;
    case (TK_INT):
          return PT_INT;
          break;
    case (TK_FLOAT):
          return PT_FLOAT;
          break;
    case (TK_STRING):
          return PT_STRING;
          break; 
    case (TK_KW): 
          if ((strcmp(token->attribute, "None")) == 0)
          { 
            return PT_NONE;
            break;
          }
          return -2;
          break;
    case (TK_EOL):
          return PT_EOL;
          break; 
    default: 
          return -2;     
  }
}
 
data_type getDataType(Token *token)
{
      if (token->type == TK_INT)
      {
            return TYPE_INT;
      }
      else if ( token->type == TK_FLOAT)
      {
            return TYPE_FLOAT;
      }
      else if (token->type == TK_STRING)
      {
            return TYPE_STRING;
      } else if (token->type == TK_ID)
      {
        tData* identifier; 
        // zavolanie funkcie SymTableSearch - a overenie čo vrátila 
      
      }
      // TODO možno NONE zmenené na iné, skontrolovať s tab. symbolov
      return TYPE_NONE; 
      
}

int checkSematics(pRules rule, exprStack* sym1, exprStack* sym2, exprStack* sym3 )
{
      bool retypeSym1 = false;
      bool retypeSym3 = false; 

      if ( rule == PR_OPERAND)
      {
            if ( sym1->dType == TYPE_NONE ) 
            {
                  return SEM_TYPE_ERROR;
            }
      }
      
      if ( rule == PR_BIB)
      {
            if (sym2->dType == TYPE_NONE)
            {
                  return SEM_TYPE_ERROR;
            }
      }

      if ( rule != PR_OPERAND || rule !=PR_BIB)
      {
            if (sym1->dType == TYPE_NONE )
            {
                  return SEM_TYPE_ERROR;
            }
            if (sym3->dType == TYPE_NONE)
            {
                  return SEM_TYPE_ERROR;
            }
      }

      // TODO ostatné pravidlá 

}


int checkDivisionByZero(Token *token)
{
  Token *nextToken;
  get_next_token(nextToken,1);

  if ( (strcmp(nextToken -> attribute, "0")) == 0)
  {
    return DIVISION_BY_ZERO_ERROR;
  }

  return OK;

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

      if (token->type == TK_DIV)
      {
            int div = checkDivisionByZero(token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(eList);
                  fprintf(stderr, "Div with zero.");
                  return SYNTAX_ERROR;
            }
      } 
      
      if (token->type == TK_DIV_DIV)
      {
            int div = checkDivisionByZero(token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(eList);
                  fprintf(stderr, "Div Div with zero.");
                  return SYNTAX_ERROR;
            }
      }
      listInsertAct(eList,token);
      
  }
  while ( token->attribute != TK_EOL);

  if ( leftBracket != rightBracket) 
  {
      fprintf(stderr, "Number of left brackets doesnt match number of right brackets");
      listDispose(eList);
      return SYNTAX_ERROR;
  }

  // TODO into a while 
  // index to the table  
  int indexStack=getIndex(token);
  if ( indexStack == -1)
  {
      listDispose(eList);
      return INTERNAL_ERROR;
  }
  if ( indexStack == -2)
  {
      listDispose(eList);
      return SYNTAX_ERROR;
  }
  // TODO save token the stack

  // index of another token
  token = get_next_token(token,0);
  int indexInput=getIndex(token);
  if ( indexInput == -1)
  {
      listDispose(eList);  
      return INTERNAL_ERROR;
  }
  if ( indexInput == -2)
  {
      listDispose(eList);
      return SYNTAX_ERROR;
  }
  
  // check the symbol in precedence table 
  char symbol = precedenceTable[indexStack][indexInput];
  // # not defined sequence of operands and operators in expression, so syntax error
  if ( (strcmp(symbol, "#") )== 0)
  {
      return SYNTAX_ERROR;
  }

  // TODO get the rule and push it to the stack
  // TODO continue with other tokens 
}