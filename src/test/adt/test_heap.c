#include "adt/heap.h"

int cmp(int a, int b) {
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    }
    return 0;
}

HEAP(int, 10, cmp)

void test_heap() {
}