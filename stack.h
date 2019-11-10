/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief Stack interface.
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */
#ifndef _STACK_H
#define _STACK_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int *data;
    int size;
    int top;
} Stack;

/**
 * @brief      Inicializes the dynamic stack
 *
 * @param      stack  Pointer to allocated stack struct
 */
void s_init(Stack *stack);

/**
 * @brief      Deallocates the memory given todynamic stack
 *
 * @param      stack  Pointer to deleded stack
 */
void s_destroy(Stack *stack);

/**
 * @brief      Resizes the dynamic stack, capacity will be increased by 10 data elements
 *
 * @param      stack  Pointer to resized stack
 */
void s_resize(Stack *stack);

/**
 * @brief      Determines whether the specified stack is empty.
 *
 * @param      stack  Pointer to the stack
 *
 * @return     True if the specified stack is empty, False otherwise.
 */
int is_empty(Stack *stack);

/**
 * @brief      Determines whether the specified stack is full.
 *
 * @param      stack  Pointer the the stack
 *
 * @return     True if the specified stack is full, False otherwise.
 */
int is_full(Stack *stack);

/**
 * @brief      Pushes new value to the top of the stack, if stack is full it will be resized
 *
 * @param      stack  Pointer to the stack
 * @param[in]  data   The integer data 
 */
void s_push(Stack *stack, int data);

/**
 * @brief      Returns the value at the top of the stack
 *
 * @param      stack  Pointer to stack
 *
 * @return     Integer value at the top of the stack
 */
int s_top(Stack *stack);

/**
 * @brief      Removes and returns the value from to the top of the stack
 *
 * @param      stack  Pointer to the stack
 *
 * @return     The removed value from the top of the stack
 */
int s_pop(Stack *stack);

#endif //_STACK_H