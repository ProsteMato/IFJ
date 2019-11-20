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


void listInitialize(exprList *eList)
{
    eList->first = NULL;
    eList->act = NULL;
    eList->last = NULL;
}

int listInsertFirst(exprList *eList, Token* token)
{
    item newItem = (struct listItem*) malloc (sizeof(struct listItem));
    if (newItem == NULL)
    {
        sprintf(stderr, "Error while malloc");
        return INTERNAL_ERROR;
    }

    newItem->lptr= NULL;
    newItem->rptr = NULL;
    newItem->token=token;

    if (eList->first != NULL)
    {
        return OTHER_ERROR;
    }
    eList->first = newItem;
    eList->act = newItem;
    eList->last = newItem;
    return OK;
}

int listInsertAct(exprList *eList, Token* token)
{
    if (eList->act == NULL)
    {
        return OTHER_ERROR;
    }

    item newItem = (struct listItem*) malloc (sizeof(struct listItem));
    if (newItem == NULL)
    {
        sprintf(stderr, "Error while malloc");
        return INTERNAL_ERROR;
    }

    newItem->lptr= NULL;
    newItem->rptr = NULL;
    newItem->token=token;

    

}