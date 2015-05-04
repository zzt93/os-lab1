#include "adt/list.h"
#include "assert.h"

struct T {
    int a;
    char c;
    ListHead link;
    double d;
} test, *t;

void test_list_entry() {
    t = & test;
    printk("t is %x\n", t);
    printk("link is %x\n", list_entry(&(t->link), struct T, link));
    printk("t is %x by a\n", list_entry(&(t->a), struct T, a));
}