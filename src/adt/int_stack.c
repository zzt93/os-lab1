#include "adt/int_stack.h"
#include "const.h"
#include "kernel/semaphore.h"

LINKLIST_IMPL(Stack, 50)

static Stack_t stack_pointer = NULL;

void init(Stack_t s, char c) {
    s->c = c;
}

void push(int c) {
    lock();
    if (stack_pointer == NULL) {
        stack_pointer = Stack_new();
    } else {
        Stack_t now = Stack_new();
        Stack_insert(NULL, stack_pointer, now);
        stack_pointer = now;
    }
    unlock();
    // initailize stack
    init(stack_pointer, c);
}

int pop() {
    lock();
    NOINTR;
    if (empty()) {
        assert(0);
    }
    Stack_t next = Stack_next(stack_pointer);
    char res = stack_pointer->c;
    Stack_remove(stack_pointer);
    Stack_free(stack_pointer);
    stack_pointer = next;
    unlock();
    return res;
}

int empty() {
    return stack_pointer == NULL;
}
