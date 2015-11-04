#ifndef __MAP_H__
#define __MAP_H__

#include "binary_tree.h"
#include "lib/malloc.h"
#include "assert.h"

#include "kernel/semaphore.h"


/**
   Using the binary search tree to implement map,
   the key of map must can be compared by
   '<'
   '>'
   '='

   Value might as well be a pointer, for the behaviour of
   copying of v is shallow copy

   Duplicate value:
   put duplicate key( which is decided by the function when initialize
   it) will replace the old value
*/

/**
   Why call left in get then interrupt then cause page fault
   -- for the `call lock()` in asm_do_irq change the value of
   $eax, $edx
*/
#define MAP(K, V, name)                                     \
    typedef struct {                                        \
        K k;                                                \
        V v;                                                \
    } Entry;                                                \
                                                            \
    static Entry name##_aim;                                \
    static int map_size = 0;                                \
                                                            \
    void name##_init_entry(Entry* e, K k, V v) {            \
        e->k = k;                                           \
        e->v = v;                                           \
    }                                                       \
                                                            \
    static int cmp_k(Entry* e1, Entry* e2) {                \
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
        lock();                                             \
        name##_aim.k = k;                                   \
        TNode_##name* e = find_fa(&name##_aim);             \
        if (e == NULL) {                                    \
            printk(RED"no such key"RESET);                  \
            return (V)NULL;                                 \
        }                                                   \
        Entry* le = NULL;                                   \
        if (left(e)) {                                      \
            le = left(e)->t;                                \
        }                                                   \
        Entry* ri = NULL;                                   \
        if (right(e)) {                                     \
            ri = right(e)->t;                               \
        }                                                   \
        unlock();                                           \
        if (le != NULL && le->k == k) {                     \
            return le->v;                                   \
        } else if ((ri != NULL && ri->k == k)){             \
            return ri->v;                                   \
        }                                                   \
        printk(RED"no such key "RESET);                     \
        return (V)NULL;                                     \
    }                                                       \
                                                            \
    int name##_put(K k, V v) {                              \
        Entry* e = kmalloc(sizeof(Entry));                  \
        if(e == NULL) {                                     \
            return 0;                                       \
        }                                                   \
        name##_init_entry(e, k, v);                         \
        lock();                                             \
        if (name##_has(e)) {                                \
            TNode_##name* t = name##_get_node(e);           \
            assert(t != NULL);                              \
            assert((t->t)->k = k);                          \
            t->t = e;                                       \
        } else {                                            \
            map_size++;                                     \
            name##_add(e);                                  \
        }                                                   \
        unlock();                                           \
        return 1;                                           \
    }                                                       \
                                                            \
    int name##_update(K k, V v) {                           \
        return name##_put(k, v);                            \
    }                                                       \
                                                            \
    int name##_remove(K k) {                                \
        lock();                                             \
        name##_aim.k = k;                                   \
        int res = name##_delete(&name##_aim);               \
        unlock();                                           \
        return res;                                         \
    }                                                       \
                                                            \
    int name##_get_map_size() {                             \
        return map_size;                                    \
    }                                                       \
                                                            \
    static int values_container_c;                          \
    static V *values_container;                             \
    /*method for in order traversal call back*/             \
    static                                                  \
    void adder(Entry *e) {                                  \
        values_container_c --;                              \
        values_container[values_container_c] = e->v;        \
    }                                                       \
                                                            \
    /* !!the content is put reversed in container */        \
    void name##_values(V *v, int *capacity) {               \
        lock();                                             \
        values_container = v;                               \
        values_container_c = *capacity;                     \
        name##_in_order(adder, capacity);                   \
        unlock();                                           \
    }                                                       \


#endif /* __MAP_H__ */