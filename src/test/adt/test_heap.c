#include "assert.h"
#include "adt/heap.h"

static int cmp(int a, int b) {
    if (a < b) {
        return 1;
    } else if (a > b) {
        return -1;
    }
    return 0;
}

#define SIZE 10

HEAP(int, SIZE, cmp, test)

void three_test() {
    assert(test_max() == 0);
    int i;
    for (i = 0; i < 3; i++) {
        assert(test_pop_max() == i);
    }
}

void test_heap() {
    int i;
    for (i = SIZE - 1; i >= 0; i--) {
        test_add(i);
        assert(_heap_size == SIZE - i);
    }
    assert(test_max() == 0);
    for (i = 0; i < SIZE; i++) {
        assert(_heap_size == SIZE - i);
        assert(test_pop_max() == i);
    }
    test_add(2);test_add(0);test_add(1);
    three_test();
    test_add(1);test_add(2);test_add(0);
    three_test();
    test_add(1);test_add(0);test_add(2);
    three_test();
    test_add(0);test_add(1);test_add(2);
    three_test();
    test_add(0);test_add(2);test_add(1);
    three_test();
}