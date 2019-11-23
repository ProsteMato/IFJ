/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */

// TODO scanner bol upravený, uprav get token !!! 

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

stackTop stack;

int getIndex(Token *token)
{
  // zisti error
  if (token == NULL)
   {
     return -1;
   }

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
          return PT_NONE;
          break;
    case (TK_EOL):
          return PT_DOLLAR;
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
        return TYPE_UNDEFINED;
      }
      // zistiť dátový typ pre kľúčové slovo None 
      // TODO možno NONE zmenené na iné, skontrolovať s tab. symbolov
      return TYPE_UNDEFINED; 
      
}

int checkSematics(pRules rule, exprStack* sym1, exprStack* sym2, exprStack* sym3 )
{
      bool retypeSym1 = false;
      bool retypeSym3 = false; 
      // TODO prípadne upraviť to NONE na iné, ak to Zuzka zmení
      // ošetriť to aj pre "dátový typ" pre kľúčové slovo None (asi TYPE_UNDEFINED)
      if ( rule == PR_OPERAND)
      {
            if ( sym1->dType == TYPE_UNDEFINED ) 
            {
                  return SEM_TYPE_ERROR;
            }
      }
      
      if ( rule == PR_BIB)
      {
            if (sym2->dType == TYPE_UNDEFINED)
            {
                  return SEM_TYPE_ERROR;
            }
      }

      if ( rule != PR_OPERAND || rule !=PR_BIB)
      {
            if (sym1->dType == TYPE_UNDEFINED )
            {
                  return SEM_TYPE_ERROR;
            }
            if (sym3->dType == TYPE_UNDEFINED)
            {
                  return SEM_TYPE_ERROR;
            }
      }

      switch (rule) 
      {
            case (PR_EPLUSE): 
                  if (sym1->dType == TYPE_STRING && (sym3->dType == TYPE_INT || sym3->dType == TYPE_FLOAT ))
                  {
                        return SEM_TYPE_ERROR;
                  }

                  if ((sym1->dType == TYPE_INT || sym1->dType == TYPE_FLOAT ) && sym3->dType == TYPE_STRING)
                  {
                        return SEM_TYPE_ERROR;
                  }

                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  break;

            case (PR_EMINUSE): 
                  if (sym1->dType == TYPE_STRING || sym3->dType == TYPE_STRING)
                  {
                        return SEM_TYPE_ERROR;
                  }
                  
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  break;

            case (PR_EMULTE):
                  if (sym1->dType == TYPE_STRING || sym3->dType == TYPE_STRING)
                  {
                        return SEM_TYPE_ERROR;
                  }
                  
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  break;

            case (PR_EDIVE): 
                  if (sym1->dType == TYPE_STRING || sym3->dType == TYPE_STRING)
                  {
                        return SEM_TYPE_ERROR;
                  }
                  
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  break;
            
            case (PR_EDIVDIVE): 
                  if (sym1->dType == TYPE_STRING || sym3->dType == TYPE_STRING)
                  {
                        return SEM_TYPE_ERROR;
                  }

                  if (sym1->dType == TYPE_FLOAT || sym3->dType == TYPE_FLOAT)
                  {
                        return SEM_TYPE_ERROR;
                  }
                  break;
            
            case (PR_ELESSE):
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  } 
                  else if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  else if ( sym1->dType != sym3->dType )
                  {
                        return SEM_TYPE_ERROR;
                  }
                  break;

            case (PR_ELESSEQE):
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  } 
                  else if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  else if ( sym1->dType != sym3->dType )
                  {
                        return SEM_TYPE_ERROR;
                  }
                  break;
            
            case (PR_EGREATE):
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  } 
                  else if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  else if ( sym1->dType != sym3->dType )
                  {
                        return SEM_TYPE_ERROR;
                  }
                  break;

            case (PR_EGREATEQE): 
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  } 
                  else if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  else if ( sym1->dType != sym3->dType )
                  {
                        return SEM_TYPE_ERROR;
                  }
                  break;
            //TODO == a != 
            //case (PR_EEQE):
            // case (PR_ENEQE):     
            default: 
                  break;
      }

      //TODO 
      // generovanie kódu a vrámci toho pretypovanie 

      return OK;
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


exprList* createList(Token* token, int* error)
{
  int leftBracket = 0;
  int rightBracket =0; 
  exprList* eList;
  pTable symbol= getIndex(token);
  data_type dType= getDataType(token);
  listInsertFirst(eList,symbol,dType);
  
  // maybe insert this to function eList createList(eList* eList, Token* token, int* error)
  // Load tokens into list 
  do 
  {
      token=get_next_token(token,0); 
      pTable symbol= getIndex(token);
      if (symbol == -1 )
      {
            listDispose(eList);
            error= INTERNAL_ERROR;
            return NULL;
      }
      data_type dType= getDataType(token);
      if (token->type == TK_KW)
      {
            if ((strcmp(token->attribute, "None")) != 0)
            { 
                  listDispose(eList);
                  error = SYNTAX_ERROR;
                  return NULL;
            }
      }
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
                  error= SYNTAX_ERROR;
                  return NULL;
            }
      } 
      
      if (token->type == TK_DIV_DIV)
      {
            int div = checkDivisionByZero(token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(eList);
                  fprintf(stderr, "Div Div with zero.");
                  error = SYNTAX_ERROR;
                  return NULL;
            }
      }
      listInsertAct(eList,symbol, dType);
      
  }
  while ( token->attribute != TK_EOL);

  if ( leftBracket != rightBracket) 
  {
      fprintf(stderr, "Number of left brackets doesnt match number of right brackets");
      listDispose(eList);
      error = SYNTAX_ERROR;
      return NULL;
  }
  return OK;
}

int callExpression(Token *token)
{
  // so far, we don't know if the operand is relational 
  isRelational = false;
  int error =0;
  // creating the list - loading tokens from input
  exprList* eList= createList(token, error);

  sInit(&stack);

  sPush(&stack, PT_DOLLAR, TYPE_UNDEFINED);
  int indexStack = PT_DOLLAR;
  int indexInput = -1 ; // no symbol in precedence table 
  exprStack* symbol;
  
 do 
 {
      if (symbol == NULL )
      {
            listDispose(eList);
            return INTERNAL_ERROR;
      }
      
      indexInput = eList->act->symbol;

      switch(precedenceTable[indexStack][indexInput])
      {
            case ('='):
                  sPush(&stack, indexInput, eList->act->dType);
                  eList->act=eList->act->rptr;
                  break;
            //case ('<'):
                  //s
            default: 
                  break;
      }





 } while (eList->act->rptr != NULL);
  
  // index to the table  
  /** int indexStack=getIndex(eList);
  if ( indexStack == -1)
  {
      listDispose(eList);
      return INTERNAL_ERROR;
  }

  eList->act= eList->act->rptr;
  int indexInput=getIndex(eList);
  if ( indexInput == -1)
  {
      listDispose(eList);  
      return INTERNAL_ERROR;
  }
  
  
  // check the symbol in precedence table 
  char symbol = precedenceTable[indexStack][indexInput];
  // # not defined sequence of operands and operators in expression, so syntax error
  if ( (strcmp(symbol, "#") )== 0)
  {
      listDispose(eList);
      return SYNTAX_ERROR;
  }
*/
  // TODO get the rule and push it to the stack
  // TODO continue with other tokens 
}