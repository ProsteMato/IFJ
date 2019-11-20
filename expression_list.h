/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * 
 * @brief Double linked list for expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */


#ifndef _EXPRESSION_LIST_H
#define _EXPRESSION_LIST_H

 #include "scanner.h"

typedef struct listItem {          
        Token *token;                                          
        struct listItem *lptr;          
        struct listItem *rptr;        
} *item;

typedef struct {
    item first;
    item act;
    item last;
} exprList;


/**
 * @brief Initialization of list 
 * 
 * @param eList to initialize 
 */
void listInitialize(exprList *eList);

/**
 * @brief Insert first token 
 * 
 * @param eList list to insert 
 * @param token token to insert into a list 
 * 
 * @return INTERNAL ERROR when somethng goes wrong with malloc
 *          OK if everything was OKAY
 *          OTHER_ERRROR when list isnt empty
 */
int listInsertFirst(exprList *eList, Token* token);

/**
 * @brief Insert token after active one and new token becomes active one 
 * 
 * @param eList list to insert 
 * @param token token to insert 
 * 
 * @return INTERNAL_ERROR when something goes wrong with malloc 
 *          OTHER_ERROR no active item in list 
 *          OK everything OKAY  
 **/
int listInsertAct(exprList *eList, Token* token);



#endif //_EXPRESSION_LIST_H