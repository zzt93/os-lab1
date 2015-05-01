#include "adt/binary_tree.h"
#include "assert.h"


int f(int* a, int *b) {
    if(*a < *b) {
        return -1;
    } else if (*a > *b) {
        return 1;
    }
    return 0;
}

BI_TREE(int*, t, f)

void test_tree_add() {
}