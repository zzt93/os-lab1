#include "int_stack.h"
#include "const.h"

LINKLIST_IMPL(Stack, 50)

static Stack_t head = NULL;

void init(Stack_t s, char c) {
    s->c = c;
}

void push(int c) {
    if (head == NULL) {
        head = Stack_new();
    } else {
        Stack_t now = Stack_new();
        Stack_insert(NULL, head, now);
        head = now;
    }
    // initailize stack
    init(head, c);
}

int pop() {
    if (empty()) {
        assert(0);
    }
    Stack_t next = Stack_next(head);
    char res = head->c;
    Stack_remove(head);
    Stack_free(head);
    head = next;
    return res;
}

int empty() {
    return head == NULL;
}