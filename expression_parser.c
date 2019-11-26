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
 
data_type getDataType(Token *token)
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
  data_type dType= getDataType(&token);
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
      data_type dType= getDataType(&token);
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
      if ( top->symbol != PT_SHIFT)
      {
            num = 1; 
      }
      do
      {
            top= top->next; 
            num += 1;  
      } while (top->symbol != PT_SHIFT);

      return num; 
}

int callExpression(Token *token)
{
  void tkq_init(); 
  int leftBracket = 0;
  int rightBracket =0; 
  pTable symbol= getIndex(token);
  exprList eList;
  data_type dType= getDataType(token);
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
      data_type dType= getDataType(token);
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
  sPush(&stack, PT_DOLLAR, TYPE_UNDEFINED);
  

  pTable indexStack = PT_DOLLAR;
  pTable indexInput; 
 // exprStack* symbol;
  exprStack* sym1 = NULL;
  exprStack* sym2 = NULL;
  exprStack* sym3 = NULL;
  
  eList.act = eList.first;

  
 //do 
 //{    
 for (int i = 0;i <3 ; i++)
  { 
      if (eList.act == NULL )
      {
            listDispose(&eList);
            disposeStack(&stack);
            return INTERNAL_ERROR;
      }
      printf("Aktívny symbol v liste je: %d \n ", eList.act->symbol);
      indexInput = eList.act->symbol;
      indexStack=stack.top->symbol;
      printf("IndexInput %d \n ", indexInput);
      printf("IndexStack %d \n ", indexStack);
     
      
      switch(precedenceTable[indexStack][indexInput])
      { 
            // equal
            case ('='):
                  sPush(&stack, indexInput, eList.act->dType);
                  eList.act=eList.act->rptr;
                  printf("Token is of ptable symbol: %d \n", symbol);
                  printf("Symbol in analysis is ' = ' \n");
                  break;
            // shift
            case ('<'):
                   printf("token on stack is of ptable symbol %d \n", indexStack); 
                  printf("Token on input is of ptable symbol: %d \n", indexInput);
                  printf("Symbol in analysis is ' < ' \n");
                  if (indexStack == PT_DOLLAR || indexStack != PT_E)
                  {
                        sPush(&stack, PT_SHIFT, TYPE_UNDEFINED);
                        sPush(&stack, indexInput, eList.act->dType);
                  }
                  sPop(&stack);
                  sPush(&stack,PT_SHIFT, TYPE_UNDEFINED);
                  indexStack=stack.top->symbol;
                  printf("Symbol on top of stack is: %d \n", indexStack);
                  printf("Should be < so, 19 \n");
                  sPush(&stack,PT_E, TYPE_UNDEFINED);
                  indexStack=stack.top->symbol;
                  printf("Symbol on top of stack is: %d \n", indexStack);
                  printf("Should be E, so 23\n");
                  eList.act = eList.act->rptr;
                 
                  break;
            // reduce
            case('>'):
                  
                  int num = symbolsToReduce();
                  printf("Number of symbols to reduce is: %d \n ", num);
                  if ( num != 1 && num != 3 )
                  {
                        listDispose(&eList);
                        disposeStack(&stack);
                        return OTHER_ERROR;
                  }
                  sym1 = stack.top->next->next;
                  sym2 = stack.top->next;
                  sym3 = stack.top;
                  pRules rule = findRule(num, sym1, sym2, sym3);
                  if ( rule == PR_NOTARULE)
                  {
                        listDispose(&eList); 
                        disposeStack(&stack);
                        return SYNTAX_ERROR;
                  }
                  int checkSem = checkSematics(rule,sym1,sym2,sym3);
                  if ( checkSem == SEM_TYPE_ERROR)
                  {
                        listDispose(&eList);
                        disposeStack(&stack);
                        return SEM_TYPE_ERROR;
                  }
                  do 
                  {
                        sPop(&stack);
                  } while (stack.top->symbol != PT_SHIFT); // shift pravdepodobne ostáva teraz na stacku, treba upraviť 
                  sPush(&stack, PT_E, TYPE_UNDEFINED);
                  printf("Token is of ptable symbol: %d \n", symbol);
                  printf("Symbol in analysis is ' > ' \n");
                  break;
            case ('#'): 
                  printf("Token is of ptable symbol: %d \n", symbol);
                  printf("Symbol in analysis is ' # ' \n");
                  listDispose(&eList);
                  disposeStack(&stack);
                  return SYNTAX_ERROR;
            default: 
                  break;
      }  
  }
 //} while (stack.top->symbol != PT_DOLLAR && indexInput != PT_DOLLAR);

 //listDispose(&eList);
 //disposeStack(&stack);
 return OK;
}