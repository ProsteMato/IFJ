/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * @file expression_stack.c
 * @brief Stack for expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */

#include "expression_stack.h" 


int sInit(stackTop* eStack)
{
    if ( eStack == NULL)
    {
        fprintf(stderr, "Wrong parameter.");
        return INTERNAL_ERROR;
    }
    eStack->top = NULL;
    return OK;
}

exprStack* sTop(stackTop* eStack)
{
    return eStack->top;
}

int sPush(stackTop* eStack, pTable symbol, Data_type dType)
{
    exprStack* new; 
    new = (exprStack*) malloc(sizeof(exprStack));
    if ( new == NULL)
    {
        return INTERNAL_ERROR;
    }

    new->next = eStack->top;
    new->symbol = symbol; 
    new->dType = dType;

    eStack->top = new;
    return OK; 
}

int sPop(stackTop* eStack)
{
    if (eStack->top != NULL)
    {
        exprStack* toPop = eStack->top;
        eStack->top = toPop->next;
        free (toPop);
        return OK;
    }
    return INTERNAL_ERROR;
}