#include "kernel/manager/MM.h"
#include "assert.h"

static inline void aligned_to_page(void *p) {
    assert(((uint32_t)p & 0xfff) == 0);
}


static void t1() {
    void *p1 = alloc_page();
    aligned_to_page(p1);
    void *p2 = alloc_page();
    aligned_to_page(p2);
    void *p3 = alloc_page();
    aligned_to_page(p3);
    void *p4 = alloc_page();
    aligned_to_page(p4);
    int *p5 = alloc_page();
    aligned_to_page(p5);

    free_page(p1);
    free_page(p2);
    free_page(p3);
    free_page(p4);
    free_page(p5);
}

static void t2() {
    void *p1 = alloc_page();
    aligned_to_page(p1);
    void *p2 = alloc_page();
    aligned_to_page(p2);
    void *p3 = alloc_page();
    aligned_to_page(p3);
    void *p4 = alloc_page();
    aligned_to_page(p4);
    int *p5 = alloc_page();
    aligned_to_page(p5);

    free_page(p3);
    free_page(p2);
    free_page(p5);
    free_page(p4);
    free_page(p1);
}

static void t3() {
    void *p1 = alloc_page();
    aligned_to_page(p1);
    void *p2 = alloc_page();
    aligned_to_page(p2);
    void *p3 = alloc_page();
    aligned_to_page(p3);
    void *p4 = alloc_page();
    aligned_to_page(p4);
    int *p5 = alloc_page();
    aligned_to_page(p5);

    free_page(p5);
    free_page(p4);
    free_page(p3);
    free_page(p2);
    free_page(p1);

    int i;
    for (i = 0; i < 33; i++) {
        alloc_page();
    }
}

void t4() {
    void *p1 = alloc_page();
    aligned_to_page(p1);
    void *p2 = alloc_page();
    aligned_to_page(p2);
    free_page(p1);
    void *p3 = alloc_page();
    aligned_to_page(p3);
    free_page(p2);
    void *p4 = alloc_page();
    aligned_to_page(p4);
    int *p5 = alloc_page();
    aligned_to_page(p5);

    free_page(p5);
    free_page(p4);
    free_page(p3);
}

void test_page_alloc() {
    t1();
    t2();
    t4();
    t3();
}