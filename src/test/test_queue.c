#include "adt/queue.h"
#include "assert.h"

QUEUE(int, 20)

void test_queue() {
    printk("----------test queue----------\n");
    enqueue(1);
    assert (dequeue() == 1);
    enqueue(2);
    assert (dequeue() == 2);
    enqueue(3);
    assert (dequeue() == 3);
}

void test_queue_capacity() {
    printk("----------test queue capacity------\n");
    unsigned int i = 0;
    for (; i < 20; ++i) {
        enqueue(i);
    }
    if (is_empty()) {
        assert(false);
    }
    assert(dequeue() == 0);
    enqueue(20);
    assert(dequeue() == 1);
    enqueue(21);
}