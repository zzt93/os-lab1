#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include "assert.h"
#include "list.h"
#include "tree.h"

#define BI_TREE(T, name)                        \
    typedef struct {                            \
        T t;                                    \
        NodeLink node;                          \
    } TreeNode;                                 \
                                                \
                                                \
    static TreeNode head;                       \
                                                \
    void name##_add(T t) {                      \
    }                                           \
                                                \
    bool name##_has(T t) {                      \
        return false;                           \
    }                                           \
                                                \
    void name##_delete(T t) {                   \
    }                                           \

#endif /* __BINARY_TREE_H__ */