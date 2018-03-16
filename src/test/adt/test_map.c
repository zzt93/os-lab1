#include "adt/map.h"
#include "lib/string.h"
#include "assert.h"

MAP(int, char*, num)

static void print_tree(TNode_num *root) {
    if (root == NULL) {
        return;
    }
    print_tree(left(root));
    printk("t is %d\n", (root->t)->k);
    print_tree(right(root));

}

char *const one_s = "1";
char *const two_s = "2";
char *const thr_s = "3";

void test_put() {
    num_put(1, one_s);
    print_tree(left(num_head));
    num_put(2, two_s);
    print_tree(left(num_head));
    num_put(3, thr_s);
    print_tree(left(num_head));
}

void test_get() {
    print_tree(left(num_head));
    char *one = num_get(1);
    assert(strcmp(one, one_s) == 0);
    char *two = num_get(2);
    assert(strcmp(two, two_s) == 0);
    char *thr = num_get(3);
    assert(strcmp(thr, thr_s) == 0);
}

void test_map() {
    test_put();

    test_get();
    //assert(one == NULL && two == NULL && thr == NULL);

    int count = 5;
    char *numbers[count];
    int res = num_values(numbers, count);
    assert(res == 3);
    int i;
    for (i = 0; i < count; i++) {
        printk("%s ", numbers[i]);
    }
}