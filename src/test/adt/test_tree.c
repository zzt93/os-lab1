#include "adt/binary_tree.h"
#include "assert.h"


static int f(int* a, int *b) {
    if(*a < *b) {
        return -1;
    } else if (*a > *b) {
        return 1;
    }
    return 0;
}

BI_TREE(int*, f, t)

static void print_tree(TNode_t* root) {
    if (root == NULL) {
        return;
    }
    print_tree(left(root));
    printk("t is %d\n", *(root->t));
    print_tree(right(root));

}

void test_tree() {
    int *a, *b, *c, *d, *e;
    int ai = 3, bi = 2, ci = 4, di = 1, ei = 5;
    a = &ai;
    b = &bi;
    c = &ci;
    d = &di;
    e = &ei;

    printk("----------test add--------\n");
    t_add(d);
    t_add(b);
    t_add(a);
    t_add(c);
    printk("left(t_head) is %x\n", left(t_head));


    for (;;) {
        t_delete(b);
        t_add(e);
        print_tree(left(t_head));
        t_delete(e);
        t_add(b);
        print_tree(left(t_head));

        t_delete(a);
        t_add(e);
        print_tree(left(t_head));
        t_delete(e);
        t_add(a);
        print_tree(left(t_head));

        t_delete(c);
        t_add(e);
        print_tree(left(t_head));
        t_delete(e);
        t_add(c);
        print_tree(left(t_head));

        t_delete(d);
        t_add(e);
        print_tree(left(t_head));
        t_delete(e);
        t_add(d);
        print_tree(left(t_head));

    }
    /*
    printk("----------test add--------\n");
    t_add(c);
    t_add(b);
    t_add(a);
    t_add(d);
    t_add(e);
    printk("left(t_head) is %x\n", left(t_head));
    print_tree(left(t_head));
    printk("----------test has--------\n");
    printk("has a %d\n", t_has(a));
    printk("has b %d\n", t_has(b));
    printk("has c %d\n", t_has(c));
    printk("has d %d\n", t_has(d));

    printk("--------test empty------ \n");
    printk("empty %d\n", t_empty());

    printk("-------- test delete-------\n");
    */
    /* delete node without child
    t_delete(d);
    printk("has d %d\n", t_has(d));
    // delete node with one child
    // comment out t_add(a);
    t_delete(b);
    printk("has b %d\n", t_has(b));
    */
    /*
    t_delete(b);
    printk("has b %d\n", t_has(b));
    print_tree(left(t_head));
    t_delete(d);
    printk("has d %d\n", t_has(d));
    print_tree(left(t_head));
    t_delete(a);
    printk("has a %d\n", t_has(a));
    print_tree(left(t_head));
    t_delete(c);
    printk("has c %d\n", t_has(c));
    print_tree(left(t_head));
    t_delete(e);
    printk("has e %d\n", t_has(e));
    print_tree(left(t_head));
    printk("empty %d\n", t_empty());

    printk("----------test add again--------\n");
    t_add(c);
    t_add(c);
    t_add(b);
    t_add(a);
    t_add(d);
    t_add(e);
    print_tree(left(t_head));
    */
}