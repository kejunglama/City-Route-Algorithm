#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED
#include "./status/status.h"

typedef struct Box
{
    struct Box *next;
    char value[1];
} Box;

typedef struct
{
    Box *top;
    int eltSize; // size of each element
} Box, *Stack;

int isEmpty(Stack *s);
Stack *createStack(int eltSize);
status push(Stack *, void *);
status pop(Stack *);
status top(Stack *, void *); // actually void **
status destroyStack(Stack *);

#endif