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

// TODO returning rules in array

const char precedenceTable[tableSize][tableSize] = {
  // 0  , 1 ,  2,   3,   4,   5.   6,   7,   8,   9,  10,  11,  12,  13,   14,  15,   16, 17, 18
  // +    -    *    /    //   <    <=   >   >=   ==   !=   (     )   id   int  float str none   $
  { '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '#', '>'}, // +       0
  { '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // -       1 
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // *       2
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // /       3
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '#', '#', '>'}, // //      4
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '>'}, // <       5
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '>'}, // <=      6 
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '>'}, // >       7
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '#', '>'}, // >=      8
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<', '>'}, // ==      9
  { '<', '<', '<', '<', '<', '#', '#', '#', '#', '#', '#', '<', '>', '<', '<', '<', '<', '<', '>'}, // !=     10
  { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '#'}, // (      11
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // )      12
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // id     13 
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // int    14
  { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // float  15
  { '>', '#', '#', '#', '#', '>', '>', '>', '>', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // string 16 
  { '#', '#', '#', '#', '#', '#', '#', '#', '#', '>', '>', '#', '#', '>', '#', '#', '#', '#', '>'}, // none   17
  { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '#', '<', '<', '<', '<', '<', '#'}, // $      18
};

stackTop stack;

int getIndex(Token *token)
{
      void tkq_init(); 
  // zisti error
  if (token == NULL)
   {
     return -1;
   }

  switch (token->type)
  {
    case (TK_ID): 
          return PT_ID;
    case (TK_PLUS):
          return PT_PLUS;
    case (TK_MINUS):
          return PT_MINUS;
    case (TK_MULT):
          return PT_MULT;
    case (TK_DIV):
          return PT_DIV;
    case (TK_DIV_DIV):
          return PT_DIV_DIV;
    case (TK_EQUAL):
          return PT_EQ;
    case (TK_NOT_EQUAL):
          return PT_NOT_EQ;
    case (TK_LESSER):
          return PT_LESS;
    case (TK_LESSER_EQUAL):
          return PT_LESS_EQ;
    case (TK_GREATER):
          return PT_GREAT;
    case (TK_GREATER_EQUAL):
          return PT_GREAT_EQ;
    case (TK_BRACKET_L):
          return PT_LEFT_BRACK;
    case (TK_BRACKET_R):
          return PT_RIGHT_BRACK;
    case (TK_INT):
          return PT_INT;
    case (TK_FLOAT):
          return PT_FLOAT;
    case (TK_STRING):
          return PT_STRING;
    case (TK_KW):
          return PT_NONE;
    default: 
          return PT_DOLLAR; 
          break;    
  }
}
 
Data_type getDataType(Token *token)
{
      void tkq_init(); 
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
        //tData* identifier; 
        // zavolanie funkcie SymTableSearch - a overenie čo vrátila 
        return TYPE_UNDEFINED;
      }
      else if (token->type == TK_EOL || token->type == TK_EOF || token->type == TK_COLON)
      {
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
                        finalType = TYPE_FLOAT;
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                        finalType = TYPE_FLOAT;
                  }

                  if (sym1->dType == TYPE_STRING && sym3->dType == TYPE_STRING)
                  {
                        finalType=TYPE_STRING;
                  }
                  if (sym1->dType == TYPE_INT && sym3->dType == TYPE_INT)
                  {
                        finalType= TYPE_INT;
                  }
                  if ( sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_FLOAT)
                  {
                        finalType = TYPE_FLOAT;
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
                        finalType = TYPE_FLOAT;
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                        finalType = TYPE_FLOAT;
                  }
                   if (sym1->dType == TYPE_INT && sym3->dType == TYPE_INT)
                  {
                        finalType= TYPE_INT;
                  }
                  if ( sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_FLOAT)
                  {
                        finalType = TYPE_FLOAT;
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
                        finalType = TYPE_FLOAT;
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                        finalType = TYPE_FLOAT;
                  }
                   if (sym1->dType == TYPE_INT && sym3->dType == TYPE_INT)
                  {
                        finalType= TYPE_INT;
                  }
                  if ( sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_FLOAT)
                  {
                        finalType = TYPE_FLOAT;
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
                        finalType = TYPE_FLOAT;
                  }

                  if (sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_INT)
                  {
                        retypeSym3 = true; 
                        finalType = TYPE_FLOAT;
                  }
                   if (sym1->dType == TYPE_INT && sym3->dType == TYPE_INT)
                  {
                        finalType= TYPE_INT;
                  }
                  if ( sym1->dType == TYPE_FLOAT && sym3->dType == TYPE_FLOAT)
                  {
                        finalType = TYPE_FLOAT;
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
                  finalType = TYPE_INT;
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
                  finalType = TYPE_UNDEFINED;
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
                  finalType = TYPE_UNDEFINED;
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
                  finalType = TYPE_UNDEFINED;
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
                  finalType = TYPE_UNDEFINED;
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
                  finalType = TYPE_UNDEFINED;
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
                  finalType = TYPE_UNDEFINED;
                  break;    
            default: 
                  break;
      }

      //TODO 
      // generovanie kódu a vrámci toho pretypovanie 
      
       
      if (retypeSym1)
      {
            printf("Retyping sym1 to float\n");
      }
       
      if (retypeSym3)
      {
      
            printf("Retyping sym3 to float\n");
      }
       

      return OK;
}

/**
int checkSematics(pRules rule, exprStack* sym1, exprStack* sym2, exprStack* sym3  )
{
      int error;
      switch (rule)
      {
            case PR_EPLUSE:
            case PR_EMINUSE:
            case PR_EMULTE: 
            case PR_EDIVE: 
            case PR_EDIVDIVE: 
                  error = arithmetic_operation_check(sym1->dType, sym2->symbol, sym3->dType);
                  return error;
            case PR_ELESSE: 
            case PR_ELESSEQE: 
            case PR_EGREATE: 
            case PR_EGREATEQE: 
            case PR_EEQE: 
            case PR_ENOTEQE: 
                  error = comparison_check(sym1->dType, sym2->symbol, sym3->dType);
                  return error; 
            default: 
                  error = INTERNAL_ERROR;
                  return error;  
      }
}
*/

int checkDivisionByZero(Token *token)
{
  //Token *nextToken;
  //preload_token(nextToken);
  void tkq_init(); 
  int l = preload_token(token);
  if ( l != OK)
  {
      fprintf(stderr,"Error with getting next token.\n");
      return l; 
  }

  if ( (strcmp(token -> attribute, "0")) == 0)
  {
    return DIVISION_BY_ZERO_ERROR;
  }

  return OK;

}

/**
int createList(Token *token, exprList *eList)
{
  void tkq_init(); 
  int leftBracket = 0;
  int rightBracket =0; 
  pTable symbol= getIndex(&token);
  Data_type dType= getDataType(&token);
  if (symbol == -1 )
  {
      error = INTERNAL_ERROR;
      return NULL;
  }
  listInitialize(&eList);
  listInsertFirst(&eList,symbol,dType);
  
  // Load tokens into list 
  do 
  {
      int e = get_next_token(&token); 
      if  (e != OK)
      {     
            error= e;
            return error;
      }
      symbol= getIndex(&token);
      if (symbol == -1 )
      {
            listDispose(&eList);
            error= INTERNAL_ERROR;
            return error;
      }
      Data_type dType= getDataType(&token);
      if (token->type == TK_KW)
      {
            if ((strcmp(token->attribute, "None")) != 0)
            { 
                  listDispose(&eList);
                  error = SYNTAX_ERROR;
                  return error;
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
            int div = checkDivisionByZero(&token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(&eList);
                  fprintf(stderr, "Div with zero.");
                  error= SYNTAX_ERROR;
                  return error;
            }
      } 
      
      if (token->type == TK_DIV_DIV)
      {
            int div = checkDivisionByZero(&token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(&eList);
                  fprintf(stderr, "Div Div with zero.");
                  error = SYNTAX_ERROR;
                  return error;
            }
      }
      listInsertAct(&eList,symbol, dType);
      
  }
  while ( token->type != TK_EOL);

  if ( leftBracket != rightBracket) 
  {
      fprintf(stderr, "Number of left brackets doesnt match number of right brackets");
      listDispose(&eList);
      error = SYNTAX_ERROR;
      return NULL;
  }
  return OK;
}
*/

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
                        case (PT_E):
                              return PR_BIB;
                        default: 
                              return PR_NOTARULE;
                  }
            } 
            else if ( (sym1->symbol == PT_ID || sym1->symbol == PT_INT || sym1->symbol == PT_FLOAT || sym1->symbol == PT_STRING || sym1->symbol == PT_NONE || sym1->symbol == PT_E) && (sym3->symbol == PT_ID || sym3->symbol == PT_INT || sym3->symbol == PT_FLOAT || sym3->symbol == PT_STRING || sym3->symbol == PT_NONE || sym3->symbol == PT_E) )
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
            top= top->next; 
            num += 1;  
      } 
      return num; 
}

int callExpression(Token *token)
{
  void tkq_init(); 
  int leftBracket = 0;
  int rightBracket =0; 
  pTable symbol= getIndex(token);
  exprList eList;
  Data_type dType= getDataType(token);
  if (symbol == -1 )
  {
      return INTERNAL_ERROR;
  }
  listInitialize(&eList);
  listInsertFirst(&eList,symbol,dType);
  int e = get_next_token(token); 
  if  (e != OK)
  {     
      return e;
  } 
  // Load tokens into list, count brackets, control division by 0
   while ( token->type != TK_EOL && token->type != TK_EOF && token->type != TK_COLON)
  {
      symbol= getIndex(token);
      if (symbol == -1 )
      {
            listDispose(&eList);
            return INTERNAL_ERROR;
      }
      Data_type dType= getDataType(token);
      if (token->type == TK_KW)
      {
            if ((strcmp(token->attribute, "None")) != 0)
            { 
                  listDispose(&eList);
                  return SYNTAX_ERROR;
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
                  listDispose(&eList);
                  fprintf(stderr, "Div with zero.\n");
                  return DIVISION_BY_ZERO_ERROR;
            }
      } 
      
      if (token->type == TK_DIV_DIV)
      {
            int div = checkDivisionByZero(token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(&eList);
                  fprintf(stderr, "Div Div with zero.\n");
                  return DIVISION_BY_ZERO_ERROR;
            }
      }
      listInsertAct(&eList,symbol, dType);
     int e = get_next_token(token); 
      if  (e != OK)
      {     
            return e;
      } 
      if (token->type == TK_EOL || token->type == TK_EOF || token->type == TK_COLON )
      {
            listInsertAct(&eList, PT_DOLLAR, TYPE_UNDEFINED);
      }
      
  }

  if ( leftBracket != rightBracket) 
  {
      fprintf(stderr, "Number of left brackets doesnt match number of right brackets.\n");
      listDispose(&eList);
      return SYNTAX_ERROR;
  }
 
  int sError = sInit(&stack);
  if (sError != OK)
  {
        return sError;
  }

  pTable indexStack;
  pTable indexInput; 
  pRules rule;  
  int i=0;
  for (int j=0;  j < 100; j++)
  {
        precedenceRules[j] = -1;
  }
  eList.act=eList.first;
  sPush(&stack, PT_DOLLAR, TYPE_UNDEFINED);
  do 
  {
      indexInput = eList.act->symbol;
      indexStack= stack.top->symbol;
      if (indexStack == PT_E)
      {
            indexStack = stack.top->next->symbol;
      }
      if ( indexStack == PT_DOLLAR && indexInput == PT_DOLLAR)
      {
            break;
      }
      switch (precedenceTable[indexStack][indexInput])
      {
            case ('='):
                  sPush(&stack,indexInput,eList.act->dType);
                  eList.act = eList.act->rptr;
                  break;
            case ('<'):
                  if (stack.top->symbol == PT_DOLLAR || stack.top->symbol != PT_E)
                  {
                        sPush(&stack, PT_SHIFT, TYPE_UNDEFINED);
                        sPush(&stack, indexInput, eList.act->dType);
                  }
                  else if (stack.top->symbol == PT_E)
                  {
                        sPop(&stack); 
                        sPush(&stack, PT_SHIFT,TYPE_UNDEFINED);
                        sPush(&stack, PT_E, TYPE_UNDEFINED);
                        sPush(&stack, indexInput, TYPE_UNDEFINED);

                  }
                  eList.act=eList.act->rptr;
                  break;
            case ('>'):
                  ;
                  int num = symbolsToReduce();
                  if (num != 1 && num != 3)
                  {
                        return SYNTAX_ERROR;
                  }  
                  if (num == 1)
                  {
                        exprStack* sym1=stack.top;
                        rule =  findRule(num,sym1, NULL, NULL);
                        
                        if ( rule == PR_NOTARULE)
                        {
                              return SYNTAX_ERROR;
                        }
                        else 
                        {
                              sPop(&stack);
                              sPop(&stack);
                              sPush(&stack, PT_E, TYPE_UNDEFINED);
                        }
                  }
                  if ( num == 3)
                  {
                        exprStack* sym1 = stack.top->next->next;
                        exprStack* sym2 = stack.top->next;
                        exprStack* sym3 =  stack.top;
                        rule  = findRule (num, sym1, sym2, sym3);
                        if (rule == PR_NOTARULE)
                        {
                              return SYNTAX_ERROR;
                        }
                        else 
                        {
                              sPop(&stack);
                              sPop(&stack);
                              sPop(&stack);
                              sPop(&stack);
                              sPush(&stack, PT_E, TYPE_UNDEFINED);
                        }
                  }
                  precedenceRules[i]=rule;
                  i++;
                  break;
            default:
                  return SYNTAX_ERROR;
                  break;

            }
      } while ( stack.top->symbol != PT_DOLLAR || eList.act->symbol != PT_DOLLAR);

 listDispose(&eList);
 //disposeStack(&stack);
 return OK;
}