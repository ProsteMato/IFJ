/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief Stack implamentation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "stack.h"

void s_init(Stack *stack){
    int *data;
    data = (int *) malloc(10*sizeof(int));
    if (data == NULL){
        fprintf(stderr, "Chyba alokacie pamati\n");
        exit(1);
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
    resized_data = (int *) malloc((stack->size + 10) * sizeof(int));
    memcpy(resized_data, stack->data, sizeof(int) * (stack->top + 1));
    free(stack->data);
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
    fprintf(stderr, "Stack is empty\n");
    return -1;
}