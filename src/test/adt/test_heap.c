#include "assert.h"
#include "adt/heap.h"

int cmp(int a, int b) {
    if (a < b) {
        return 1;
    } else if (a > b) {
        return -1;
    }
    return 0;
}

#define SIZE 10

HEAP(int, 10, cmp)

void test_heap() {
    int i;
    for (i = SIZE - 1; i >= 0; i--) {
        add(i);
    }
    assert(max() == 0);
    for (i = 0; i < SIZE; i++) {
        assert(pop_max() == i);
    }
}