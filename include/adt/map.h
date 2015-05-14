#ifndef __MAP_H__
#define __MAP_H__

#include "binary_tree.h"
#include "lib/malloc.h"
#include "assert.h"


/**
   Using the binary search tree to implement map,
   the key of map must can be compared by
   '<'
   '>'
   '='
*/
#define MAP(K, V, name)                                     \
    typedef struct {                                        \
        K k;                                                \
        V v;                                                \
    } Entry;                                                \
                                                            \
    static Entry get;                                       \
                                                            \
    void init_entry(Entry* e, K k, V v) {                   \
        e->k = k;                                           \
        e->v = v;                                           \
    }                                                       \
                                                            \
    int cmp_k(Entry* e1, Entry* e2) {                       \
        if (e1->k < e2->k) {                                \
            return -1;                                      \
        } else if (e1->k > e2->k) {                         \
            return 1;                                       \
        }                                                   \
        return 0;                                           \
    }                                                       \
                                                            \
    BI_TREE(Entry*, cmp_k, name);                           \
                                                            \
                                                            \
    /*get a node using the funciton when init as criteria*/ \
    V name##_get(K k) {                                     \
        get.k = k;                                          \
        TNode_##name* e = find_fa(&get);                    \
        if (e == NULL) {                                    \
            printk(RED"no such key"RESET);                  \
            return NULL;                                    \
        }                                                   \
        Entry* le = left(e)->t ;                            \
        Entry* ri = right(e)->t;                            \
        if (le->k == k) {                                   \
            return le->v;                                   \
        } else {                                            \
            assert(ri->k == k);                             \
            return ri->v;                                   \
        }                                                   \
    }                                                       \
                                                            \
    void name##_put(K k, V v) {                             \
        Entry* e = kmalloc(sizeof(Entry));                  \
        init_entry(e, k, v);                                \
        name##_add(e);                                      \
    }                                                       \
                                                            \


#endif /* __MAP_H__ */