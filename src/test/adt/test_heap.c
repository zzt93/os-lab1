#include "assert.h"
#include "adt/heap.h"

static int cmp(int *a, int *b) {
    if (*a < *b) {
        return 1;
    } else if (*a > *b) {
        return -1;
    }
    return 0;
}

#define SIZE 10

HEAP(int, SIZE, cmp)

void three_test() {
    assert(max() == 0);
    int i;
    for (i = 0; i < 3; i++) {
        assert(pop_max() == i);
    }
}

void test_heap() {
    int i;
    for (i = SIZE - 1; i >= 0; i--) {
        add(i);
        assert(size == SIZE - i);
    }
    assert(max() == 0);
    for (i = 0; i < SIZE; i++) {
        assert(size == SIZE - i);
        assert(pop_max() == i);
    }
    add(2);add(0);add(1);
    three_test();
    add(1);add(2);add(0);
    three_test();
    add(1);add(0);add(2);
    three_test();
    add(0);add(1);add(2);
    three_test();
    add(0);add(2);add(1);
    three_test();
}