#include "adt/queue.h"
#include "assert.h"

QUEUE(int, 20, t)

void test_queue() {
    printk("----------test queue----------\n");
    t_enqueue(1);
    assert (t_dequeue() == 1);
    t_enqueue(2);
    assert (t_dequeue() == 2);
    t_enqueue(3);
    assert (t_dequeue() == 3);
}

void test_queue_capacity() {
    printk("----------test queue capacity------\n");
    unsigned int i = 0;
    for (; i < 20; ++i) {
        t_enqueue(i);
    }
    if (t_is_empty()) {
        assert(false);
    }
    assert(t_dequeue() == 0);
    t_enqueue(20);
    assert(t_dequeue() == 1);
    t_enqueue(21);
}