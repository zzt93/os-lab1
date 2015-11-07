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

static
void contiguous_test(int start) {
    int size = test_heap_size();
    int i;
    for (i = 0; i < size; i++) {
        assert(test_pop_max() == i + start);
    }
}

static
void three_test() {
    assert(test_max() == 0);
    contiguous_test(0);
}

void test_heap() {
    int i;
    for (i = SIZE - 1; i >= 0; i--) {
        test_add(i);
        assert(test_heap_size() == SIZE - i);
    }
    assert(test_max() == 0);
    for (i = 0; i < SIZE; i++) {
        assert(test_heap_size() == SIZE - i);
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

    // test heap.update()
    test_add(0);test_add(2);test_add(1);
    // set the largest to another number
    test_update(0, 3);
    // should change to 1, 2, 3
    contiguous_test(1);

    // test percolate down
    test_add(1);test_add(0);test_add(2);test_add(3);test_add(4);
    test_add(6);test_add(5);
    test_update(0, 7);
    // should change to 1~7
    contiguous_test(1);

    // test percolate up
    test_add(1);test_add(7);test_add(2);test_add(3);test_add(4);
    test_add(6);test_add(5);
    int j = 0, num = 0;
    heap_each(j, num) {
        if (num == 7) {
            test_update(j, 0);
            break;
        }
    }
    // should change to 0~6
    contiguous_test(0);
}