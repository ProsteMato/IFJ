/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 * 
 * @file stack.c
 * @brief Stack implamentation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "stack.h"

void s_init(Stack *stack){
    int *data;
    data = (int *) malloc(10*sizeof(int));
    if (data == NULL){
        stack->data = NULL;
        return;
    }
    stack->data = data;
    stack->size = 10;
    stack->top = -1;
}

void s_destroy(Stack *stack){
    free(stack->data);
    stack->data = NULL;
    stack->size = 0;
    stack->top = -1;
}

void s_resize(Stack *stack){
    int *resized_data;
    resized_data = (int *) realloc(stack->data ,(stack->size + 10) * sizeof(int));
    stack->data = resized_data;
    stack->size = stack->size + 10;
}

int is_empty(Stack *stack){
    if (stack->top < 0){
        return 1;
    } else {
        return 0;
    }
}

int is_full(Stack *stack){
    if (stack->top + 1 == stack->size){
        return 1;
    } else {
        return 0;
    }
}

void s_push(Stack *stack, int data){
    if (is_full(stack) == 1){
        s_resize(stack);
    }
    stack->top = stack->top + 1;
    stack->data[stack->top] = data;
}

int s_top(Stack *stack){
    return stack->data[stack->top];
}

int s_pop(Stack *stack){
    if (is_empty(stack) == 0)    {
        int data = stack->data[stack->top];
        stack->top = stack->top - 1;
        return data;
    }
    return -1;
}