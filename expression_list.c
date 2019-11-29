/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Double linked list for expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */


#include "expression_list.h"
#include "error.h"
#include "expression_parser.h"


void listInitialize(exprList *eList)
{
    eList->first = NULL;
    eList->act = NULL;
    eList->last = NULL;
}

int listInsertFirst(exprList *eList, char *attribute, pTable symbol, Data_type dType)
{
    item newItem = (struct listItem*) malloc (sizeof(struct listItem));
    if (newItem == NULL)
    {
        fprintf(stderr, "Error while malloc");
        return INTERNAL_ERROR;
    }

    newItem->lptr= NULL;
    newItem->rptr = NULL;
    newItem->symbol = symbol;
    newItem->attribute = attribute;
    newItem->dType = dType;

    if (eList->first != NULL)
    {
        return INTERNAL_ERROR;
    }
    eList->first = newItem;
    eList->act = newItem;
    eList->last = newItem;
    return OK;
}

int listInsertAct(exprList *eList, char *attribute, pTable symbol, Data_type dType)
{
    if (eList->act == NULL)
    {
        return INTERNAL_ERROR;
    }

    item newItem = (struct listItem*) malloc (sizeof(struct listItem));
    if (newItem == NULL)
    {
        fprintf(stderr, "Error while malloc");
        return INTERNAL_ERROR;
    }

    newItem->symbol = symbol;
    newItem->dType = dType;
    newItem->attribute = attribute;
    newItem->lptr = eList->act;
    newItem->rptr= NULL;
    eList->act->rptr = newItem;
    eList->last = newItem;
    eList->act = newItem;
    return OK;
}

/**
Token* copyAct(exprList *eList)
{
    if (eList->act != NULL)
    {
        Token* tokenReturn = eList->act->token;
        eList->act=eList->act->rptr;
        return tokenReturn;
    }
}
*/

void listDispose(exprList *eList)
{
    item itemDelete; 
    itemDelete= eList->first;

    while (eList->first != NULL)
    {
        eList->first=eList->first->rptr;
        free(itemDelete);
        itemDelete = eList->first;
    }

    eList->first= NULL;
    eList->act = NULL;
    eList->last = NULL;
}

