#include "lib/malloc.h"
#include "lib/printk.h"

void test_malloc() {
    printk("---------test malloc -------\n");
    unsigned int i = 0;
    for (; i < 20; ++i) {
        kmalloc(i);
    }
}

void test_malloc_free() {
    printk("---------test malloc free -------\n");
    int *p1 = kmalloc(6);
    *(p1 + 1) = -1;
    *(p1 + 2) = 1;
    kfree(p1);

    int *p2 = kmalloc(7);
    *(p2 + 1) = -1;
    *(p2 + 2) = 1;
    kfree(p2);

    int *p3 = kmalloc(4);
    *(p3 + 1) = -1;
    *(p3 + 2) = 1;
    kfree(p3);
}

void test_free_in_order() {
    printk("----------test free in order------------\n");
    int *p1 = kmalloc(6);
    *(p1 + 1) = -1;
    *(p1 + 2) = 1;

    int *p2 = kmalloc(7);
    *(p2 + 1) = -1;
    *(p2 + 2) = 1;

    int *p3 = kmalloc(4);
    *(p3 + 1) = -1;
    *(p3 + 2) = 1;

    kfree(p1);
    kfree(p2);
    kfree(p3);

    // in reverse
    int *p4 = kmalloc(6);
    *(p4 + 1) = -1;
    *(p4 + 2) = 1;

    int *p5 = kmalloc(7);
    *(p5 + 1) = -1;
    *(p5 + 2) = 1;

    int *p6 = kmalloc(4);
    *(p6 + 1) = -1;
    *(p6 + 2) = 1;

    kfree(p6);
    kfree(p5);
    kfree(p4);
}

void test_free_random() {
    printk("------------test free random-------------\n");
    int *p1 = kmalloc(6);
    *(p1 + 1) = -1;
    *(p1 + 2) = 1;

    int *p2 = kmalloc(7);
    *(p2 + 1) = -1;
    *(p2 + 2) = 1;

    int *p3 = kmalloc(4);
    *(p3 + 1) = -1;
    *(p3 + 2) = 1;

    kfree(p2);
    kfree(p1);
    kfree(p3);
}

void test_malloc_free_large() {
    printk("-------------test malloc free large\n");
    int *p1 = kmalloc(600);
    *(p1 + 1) = -1;
    *(p1 + 2) = 1;

    int *p2 = kmalloc(700);
    *(p2 + 1) = -1;
    *(p2 + 2) = 1;

    int *p3 = kmalloc(4000);
    *(p3 + 1) = -1;
    *(p3 + 2) = 1;

    kfree(p1);
    kfree(p3);

    int *p4 = kmalloc(4096);
    *(p4 + 1) = 1;
    *(p4 + 2) = -1;

    kfree(p2);
    kfree(p4);
}