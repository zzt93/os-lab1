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
    static int _name##_map_size = 0;                        \
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
            unlock();                                       \
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
        Entry e1;                                           \
        name##_init_entry(&e1, k, v);                       \
        lock();                                             \
        if (name##_has(&e1)) {                              \
            /*update original one if has this entry*/       \
            TNode_##name* treenode = name##_get_node(&e1);  \
            assert(treenode != NULL);                       \
            assert((treenode->t)->k = k);                   \
            /*just set new value for this entry*/           \
            treenode->t->v = v;                             \
        } else {                                            \
            Entry* e = kmalloc(sizeof(Entry));              \
            if(e == NULL) {                                 \
                return 0;                                   \
            }                                               \
            name##_init_entry(e, k, v);                     \
            _name##_map_size++;                             \
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
        return _name##_map_size;                            \
    }                                                       \
                                                            \
    static int values_container_c;                          \
    static V *values_container;                             \
    /*method for in-order traversal call back*/             \
    static                                                  \
    void values_adder(Entry *e) {                           \
        values_container[values_container_c] = e->v;        \
        values_container_c ++;                              \
    }                                                       \
                                                            \
    static int keySet_container_c;                          \
    static K *keySet_container;                             \
    /*method for in order-traversal call back*/             \
    static                                                  \
    void keySet_adder(Entry *e) {                           \
        keySet_container[keySet_container_c] = e->k;        \
        keySet_container_c ++;                              \
    }                                                       \
                                                            \
    /*static Sem name##_values_mutex, name##_key_mutex; */  \
    /* TODO change to mutex and PV*/                        \
    /* @return how many values are returned in v*/          \
    int name##_values(V *v, int capacity) {                 \
        lock();                                             \
        values_container = v;                               \
        values_container_c = 0;                             \
        int save_c = capacity;                              \
        name##_in_order(values_adder, &save_c);             \
        unlock();                                           \
        return (capacity >= _name##_map_size)               \
            ? _name##_map_size : capacity;                  \
    }                                                       \
                                                            \
    /* @return how many values are stored in k*/            \
    int name##_keySet(K *k, int capacity) {                 \
        lock();                                             \
        keySet_container = k;                               \
        keySet_container_c = 0;                             \
        int save_c = capacity;                              \
        name##_in_order(keySet_adder, &save_c);             \
        unlock();                                           \
        return (capacity >= _name##_map_size)               \
            ? _name##_map_size : capacity;                  \
    }                                                       \


#endif /* __MAP_H__ */