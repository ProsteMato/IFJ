/**
 *  Project: Implementace prekladača imperatívneho jazýka IFJ19
 * @file expression_list.h
 * @brief Double linked list for expression parser
 * 
 * @author Magdaléna Ondrušková <xondru16@stud.fit.vutbr.cz>
 * 
 */


#ifndef _EXPRESSION_LIST_H
#define _EXPRESSION_LIST_H

 #include "expression_parser.h"

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
 * @return INTERNAL ERROR when somethng goes wrong with list
 *          OK if everything was OKAY
 */
int listInsertFirst(exprList *eList, char *attribute, pTable symbol, Data_type dType);

/**
 * @brief Insert token after active one and new token becomes active one 
 * 
 * @param eList list to insert 
 * @param token token to insert 
 * 
 * @return INTERNAL_ERROR when something goes wrong with list 
 *          OK everything OKAY  
 **/
int listInsertAct(exprList *eList, char *attribute, pTable symbol, Data_type dType);

/**
 * @brief Delete all items on the list 
 * 
 * @param eList list to delete 
 */
void listDispose(exprList *eList);

#endif //_EXPRESSION_LIST_H
