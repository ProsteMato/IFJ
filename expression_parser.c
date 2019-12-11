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
#include "main.h"

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
  { '#', '#', '#', '#', '#', '#', '#', '#', '#', '>', '>', '#', '>', '#', '#', '#', '#', '#', '>'}, // none   17
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
   // from token returns symbol in precedence table 
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


int checkDivisionByZero(Token *token)
{
  void tkq_init(); 
  int l = preload_token(token);
  if ( l != OK)
  {
      return l; 
  } 
  // if next token is 0 returns div with zero error
  if ( (strcmp(token -> attribute, "0")) == 0)
  {
    return DIVISION_BY_ZERO_ERROR;
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
                        case (PT_NONE):
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
                              return PR_EDIVE;
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


int checkSematics(pRules rule, exprStack* sym1, exprStack* sym2, exprStack* sym3  )
{
      int error;
      switch (rule)
      {
            //controls aritmetic operations 
            case PR_EPLUSE:
            case PR_EMINUSE:
            case PR_EMULTE: 
            case PR_EDIVE: 
            case PR_EDIVDIVE: 
                  error = arithmetic_operation_check(sym1->dType, rule, sym3->dType);
                  return error;
            // controls relation operations
            case PR_ELESSE: 
            case PR_ELESSEQE: 
            case PR_EGREATE: 
            case PR_EGREATEQE: 
            case PR_EEQE: 
            case PR_ENOTEQE: 
                  error = comparison_check(sym1->dType, rule, sym3->dType);
                  return error; 
            // controls symbol between brackets
            case PR_BIB: 
                  switch (sym2->symbol)
                  {
                        case (PT_E):
                        case (PT_INT):
                        case (PT_FLOAT):
                        case (PT_STRING):
                        case (PT_ID): 
                        case (PT_NONE):
                              return OK; 
                        default: 
                              return SYNTAX_ERROR;
                  }
            default: 
                  error = INTERNAL_ERROR;
                  return error;  
      }
}

Data_type getFinalType(pRules rule, exprStack* sym1, exprStack* sym2, exprStack* sym3)
{
      switch (rule)
      {
            // final type of arithmetic operation
            case PR_EPLUSE:
            case PR_EMINUSE:
            case PR_EMULTE:
            case PR_EDIVDIVE:
            case PR_EDIVE: 
                  return arithmetic_operation_return_type(sym1->dType,rule,sym3->dType);
            // final type of relational operation
            case PR_ELESSE:
            case PR_ELESSEQE:
            case PR_EGREATE:
            case PR_EGREATEQE: 
            case PR_EEQE: 
            case PR_ENOTEQE: 
                  return TYPE_BOOL;
            // final type between brackets
            case PR_BIB:
                  return sym2->dType;
            default: 
                  return TYPE_UNDEFINED;
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
  isRelational = false;
  int leftBracket = 0;
  int rightBracket =0; 
  pTable symbol= getIndex(token);
  exprList eList;
  if (symbol == -1 )
  {
      return INTERNAL_ERROR;
  }
  listInitialize(&eList);
  listInitialize(&operandList);
  bool numberinID = false;
  if (token->type == TK_EOL || token->type == TK_EOF || token->type == TK_COLON)
  {
      int ee = unget_token(token);
      if (ee != OK)
      {
            return ee;
      }
      return SYNTAX_ERROR;
  }
  
  // Load tokens into list, count brackets, control division by 0
   while ( token->type != TK_EOL && token->type != TK_EOF && token->type != TK_COLON)
  {
      if ( token->type == TK_ID && numberinID)
      {
            return SYNTAX_ERROR;
      } 
      //Control for name of id like 5a 
      if ( token->type == TK_INT || token->type == TK_FLOAT)
      {
            numberinID = true;
      }
      else numberinID = false ;
     if (token->type == TK_ID)
      {     
            if(!is_variable_defined(NULL, local_table, param_list, token->attribute)) {
                  SetCalled(root, token->attribute);
            }
            if (!is_variable_defined(root, local_table, param_list, token->attribute))
            {
                  return SEM_FUNCTION_ERROR;
            }
      } 
      symbol= getIndex(token);
      if (symbol == -1 )
      {
            listDispose(&eList);
            return INTERNAL_ERROR;
      }
      Data_type dType=  get_type_from_token(root,local_table,*token);
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

       if (token->type ==TK_ASSIGN)
      {
             return SYNTAX_ERROR;
      }
      // if there was a relational operand, it sets variable to true
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
      // control for div and divdiv with zero
      if (token->type == TK_DIV)
      {
            int div = checkDivisionByZero(token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(&eList);
                  return DIVISION_BY_ZERO_ERROR;
            }
      } 
      if (token->type == TK_DIV_DIV)
      {
            int div = checkDivisionByZero(token);
            if ( div == DIVISION_BY_ZERO_ERROR)
            { 
                  listDispose(&eList);
                  return DIVISION_BY_ZERO_ERROR;
            }
      }
      if ( token ->type == TK_COMMA)
      {
            return SYNTAX_ERROR;
      }
      // inserting into a list
      if (eList.act == NULL)
      {
            listInsertFirst(&eList, token->attribute, symbol, dType);
      } else 
      {
             listInsertAct(&eList,token->attribute, symbol, dType);
      }
      if ( symbol == PT_ID || symbol == PT_INT || symbol == PT_FLOAT || symbol == PT_STRING || symbol == PT_NONE)
      {
             if ( operandList.first == NULL)
            {
                  listInsertFirst(&operandList,token->attribute, symbol, dType);
            }
            else 
            {  
                  listInsertAct(&operandList,token->attribute, symbol, dType);
            }
      }
      // inserts the last $ into a list
      int e = get_next_token(token); 
      if  (e != OK)
      {     
            return e;
      } 
      if (token->type == TK_EOL || token->type == TK_EOF || token->type == TK_COLON )
      {
            listInsertAct(&eList,token->attribute, PT_DOLLAR, TYPE_UNDEFINED);
            
      }
      
  }
  int err_unget= unget_token(token);
  if (err_unget  != OK )
  {
      return err_unget;
  }
  // controls number of brackets
  if ( leftBracket != rightBracket) 
  {
      listDispose(&eList);
      return SYNTAX_ERROR;
  }
  // initialization of stack
  int sError = sInit(&stack);
  if (sError != OK)
  {
        return sError;
  }

  pTable indexStack;
  pTable indexInput; 
  pRules rule;  
  bool generateExpr = true;
  int i=0;
  for (int j=0;  j < 100; j++)
  {
        precedenceRules[j] = -1;
  }
  eList.act=eList.first;


  if ( eList.act ->rptr == NULL)
  {
        switch (eList.act->symbol) 
        {
            case PT_INT: 
            case PT_FLOAT:
            case PT_STRING: 
            case PT_NONE:
            case PT_ID:
                  return OK;
            default:
                  return SYNTAX_ERROR;

        }
  }
  // check if the expression has alright syntax/sematics
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
      if (precedenceTable[indexStack][indexInput] == 35 && ( indexInput == PT_NONE || indexStack == PT_NONE ))
      {
            if (stack.top->next->symbol != PT_EQ || stack.top->next->symbol != PT_NOT_EQ)
            {
                  return SEM_TYPE_ERROR;
            }
      }
      if (precedenceTable[indexStack][indexInput] == 35 && ( indexInput == PT_STRING || indexStack == PT_STRING ))
      {
            if (stack.top->next->symbol != PT_PLUS)
            {
                  return SEM_TYPE_ERROR;
            }
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
                        finalType = stack.top->dType;
                        sPop(&stack); 
                        sPush(&stack, PT_SHIFT,TYPE_UNDEFINED);
                        sPush(&stack, PT_E, finalType);
                        sPush(&stack, indexInput, eList.act->dType);

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
                              finalType= sym1->dType;
                              sPop(&stack);
                              sPop(&stack);
                              sPush(&stack, PT_E, finalType);
                        }
                  }
                  if ( num == 3)
                  {
                        exprStack* sym1 = stack.top->next->next;
                        exprStack* sym2 = stack.top->next;
                        exprStack* sym3 =  stack.top;
                        rule  = findRule (num, sym1, sym2, sym3);
                        int sematics = checkSematics(rule, sym1, sym2, sym3);
                        finalType = getFinalType(rule, sym1, sym2, sym3);
                        if (finalType != TYPE_UNDEFINED)
                        {     
                              generateExpr = false;
                               
                        }
                        if (sematics != OK)
                        {
                              return sematics;
                        }
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
                              sPush(&stack, PT_E, finalType);
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
  if (err_unget != OK)
  {
      return err_unget;
  }
  if (generate || generateExpr)  { // musia byt obe true aby sa generovalo
      int returnValue = OK;
      if((returnValue = gen_expr()) != OK){ //kontrola ci nahodou nevratil chybu
            return returnValue;
      }
  } else {
      generate = true; //aby sa nabudúce generovalo
      int returnValue = OK;
      if((returnValue = gen_clear()) != OK){ //aby na stacku neostala vyhodnotená hodnota z expr
            return returnValue;
      }
 }
 listDispose(&eList);
 return OK;
}