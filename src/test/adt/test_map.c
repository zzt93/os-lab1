#include "adt/map.h"
#include "lib/string.h"
#include "assert.h"

MAP(int, char*, num)

static void print_tree(TNode_num* root) {
    if (root == NULL) {
        return;
    }
    print_tree(left(root));
    printk("t is %d\n", (root->t)->k);
    print_tree(right(root));

}

void test_put() {
    num_put(1, "1");
    print_tree(left(num_head));
    num_put(2, "2");
    print_tree(left(num_head));
    num_put(3, "3");
    print_tree(left(num_head));
}

void test_get() {
    print_tree(left(num_head));
    char * one = num_get(1);
    assert(one[0] == '1');
    assert(strcmp(one, "1") == 0);
    char *two = num_get(2);
    assert(two[0] == '2');
    assert(strcmp(two, "2") == 0);
    char *thr = num_get(3);
    assert(thr[0] == '3');
    assert(strcmp(thr, "3") == 0);
}

void test_map() {
    num_put(1, "1");
    print_tree(left(num_head));
    num_put(2, "2");
    print_tree(left(num_head));
    num_put(3, "3");
    print_tree(left(num_head));
    
    char * one = num_get(1);
    assert(one[0] == '1');
    assert(strcmp(one, "1") == 0);
    char *two = num_get(2);
    assert(two[0] == '2');
    assert(strcmp(two, "2") == 0);
    char *thr = num_get(3);
    assert(thr[0] == '3');
    assert(strcmp(thr, "3") == 0);
    //assert(one == NULL && two == NULL && thr == NULL);
}