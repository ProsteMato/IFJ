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
    default: 
          return PT_DOLLAR; 
          break;    
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
      }
      else if (token->type == TK_KW)
      {
            return TYPE_NONE;
      }
      else if (token->type == TK_ID)
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
                  
                  if ( sym1->dType == TYPE_NONE || sym3->dType == TYPE_NONE)
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

                   if ( sym1->dType == TYPE_NONE ||sym3->dType == TYPE_NONE)
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
                  
                   if ( sym1->dType == TYPE_NONE ||sym3->dType == TYPE_NONE)
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
                  
                  if ( sym1->dType == TYPE_NONE ||sym3->dType == TYPE_NONE)
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

                   if ( sym1->dType == TYPE_NONE ||sym3->dType == TYPE_NONE)
                  {
                        return SEM_TYPE_ERROR;
                  }

                  if (sym1->dType == TYPE_FLOAT || sym3->dType == TYPE_FLOAT)
                  {
                        return SEM_TYPE_ERROR;
                  }
                  break;
            
            case (PR_ELESSE):
                  if ( sym1->dType == TYPE_NONE || sym3->dType == TYPE_NONE)
                  {
                        return SEM_TYPE_ERROR;
                  }

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
                  if ( sym1->dType == TYPE_NONE || sym3->dType == TYPE_NONE)
                  {
                        return SEM_TYPE_ERROR;
                  }

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
                  if ( sym1->dType == TYPE_NONE || sym3->dType == TYPE_NONE)
                  {
                        return SEM_TYPE_ERROR;
                  }

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
                  if ( sym1->dType == TYPE_NONE || sym3->dType == TYPE_NONE)
                  {
                        return SEM_TYPE_ERROR;
                  }

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
            case (PR_EEQE):
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  } 

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  break;

            case (PR_ENOTEQE): 
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_FLOAT)
                  {
                        retypeSym1 = true; 
                  } 

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                  }
                  break;    
            default: 
                  break;
      }
      // dočasne 
      return OK;
      //TODO 
      // generovanie kódu a vrámci toho pretypovanie 
      /** 
       * 
       * if (retypeSym1)
       * {
       * 
       * }
       * 
       * if (retypeSym3)
       * {
       * 
       * 
       * }
       */

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
      dType = getDataType(token); 
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


pRules findRule(int num, exprStack* sym1, exprStack* sym2, exprStack* sym3)
{ 
      if ( num == 1)
      {
            switch (sym1->symbol)
            {     // E-> i
                  case (PT_ID):
                        return PR_OPERAND;
                  // E-> int
                  case (PT_INT):
                        return PR_INT;
                  // E-> float
                  case (PT_FLOAT):
                        return PR_FLOAT;
                  // E-> string 
                  case (PT_STRING):
                        return PR_STRING;
                  // E-> none 
                  case (PT_NONE):
                        return PR_NONE;
                  default: 
                        return PR_NOTARULE;
            }
      } 
      else if (num == 3)
      {     // E -> (i)
            if (sym1->symbol == PT_LEFT_BRACK && sym3->symbol == PT_RIGHT_BRACK)
            {
                  switch (sym2->symbol)
                  {
                        case (PT_ID): 
                              return PR_BIB; 
                        case (PT_INT):
                              return PR_BIB;
                        case (PT_FLOAT):
                              return PR_BIB;
                        case (PT_STRING):
                              return PR_BIB;
                        default: 
                              return PR_NOTARULE;
                  }
            } 
            else if ( (sym1->symbol == PT_ID || sym1->symbol == PT_INT || sym1->symbol == PT_FLOAT || sym1->symbol == PT_STRING || sym1->symbol == PT_NONE ) && (sym3->symbol == PT_ID || sym3->symbol == PT_INT || sym3->symbol == PT_FLOAT || sym3->symbol == PT_STRING || sym3->symbol == PT_NONE ) )
            {     
                  switch (sym2->symbol)
                  {     
                        // E -> E + E
                        case (PT_PLUS):
                              return PR_EPLUSE;
                        // E -> E - E
                        case (PT_MINUS):
                              return PR_EMINUSE;
                        // E -> E * E 
                        case (PT_MULT):
                              return PR_EMULTE;
                        // E -> E / E 
                        case (PT_DIV):
                              return PR_EDIVDIVE;
                        // E -> E // E 
                        case (PT_DIV_DIV):
                              return PR_EDIVDIVE;
                        // E -> E < E 
                        case (PT_LESS):
                              return PR_ELESSE;
                        // E -> E <= E 
                        case (PT_LESS_EQ):
                              return PR_ELESSEQE;
                        // E -> E > E 
                        case (PT_GREAT):
                              return PR_EGREATE;
                        // E -> E >= E 
                        case (PT_GREAT_EQ):
                              return PR_EGREATEQE;
                        // E -> E == E 
                        case (PT_EQ):
                              return PR_EEQE;
                        // E -> E != E 
                        case (PT_NOT_EQ):
                              return PR_ENOTEQE;
                        default:
                              return PR_NOTARULE;
                  }
            } else 
            {
                  return PR_NOTARULE;
            }
      }
      else 
      {     // did not find a rule 
            return PR_NOTARULE;
      }

}

int symbolsToReduce()
{
      exprStack* top= sTop(&stack);
      int num = 0; 

      while (top->symbol != PT_SHIFT)
      {
            num += 1; 
            top= top->next; 
      }
      return num; 
}

int callExpression(Token *token)
{
  // so far, we don't know if the operand is relational 
  isRelational = false;
  int error =0;
  // creating the list - loading tokens from input
  exprList* eList;
  listInitialize(eList);
  eList=createList(token, error);

  sInit(&stack);

  sPush(&stack, PT_DOLLAR, TYPE_UNDEFINED);
  pTable indexStack = PT_DOLLAR;
  pTable indexInput = -1 ; // no symbol in precedence table 
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
            // equal
            case ('='):
                  sPush(&stack, indexInput, eList->act->dType);
                  eList->act=eList->act->rptr;
                  break;
            // shift
            case ('<'):
                  if (indexStack == PT_DOLLAR)
                  {
                        sPush(&stack, PT_SHIFT, TYPE_UNDEFINED);
                        sPush(&stack, indexInput, eList->act->dType);
                  }
                  
                  break;
            // reduce
            case('>'):
                  break;
            case ('#'): 
                  return SYNTAX_ERROR;
            default: 
                  break;
      }

 } while (eList->act->rptr != NULL);
}