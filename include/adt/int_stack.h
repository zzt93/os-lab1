#ifndef __CHAR_STACK_H__
#define __CHAR_STACK_H__

#include "adt/linklist.h"

LINKLIST_DEF(Stack)
    int c;
LINKLIST_DEF_FI(Stack)

int empty();

void push(int);

int pop();

#endif /* __CHAR_STACK_H__ */