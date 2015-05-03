#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include "assert.h"
#include "tree.h"
#include "lib/malloc.h"

/*
  T: the type parameter
  f: the comparing funciton, bool f(T, T);
  name: the name of tree

  the order:
  left < mid <= right

  the duplicate:
  if there are more than one same elements,
  no gurrantee which will be find or delete
  
  implementation detail:
  use a TreeNode* as the head, and only use the left link
*/

#define BI_TREE(T, f, name)                                             \
    typedef struct {                                                    \
        T t;                                                            \
        NodeLink link;                                                  \
    } TNode_##name;                                                     \
                                                                        \
    static TNode_##name h;                                              \
    static TNode_##name* name##_head = &h;                                     \
                                                                        \
    static inline TNode_##name* left(TNode_##name* t) {                 \
        NodeLink l = t->link;                                           \
        if (l.left == NULL) {                                           \
            return NULL;                                                \
        }                                                               \
        return ptr2container(l.left, TNode_##name, link);               \
    }                                                                   \
    static inline TNode_##name* right(TNode_##name* t) {                \
        NodeLink l = t->link;                                           \
        if (l.right == NULL) {                                          \
            return NULL;                                                \
        }                                                               \
        return ptr2container(l.right, TNode_##name, link);              \
    }                                                                   \
    static inline void set_le(TNode_##name* n, TNode_##name *l) {       \
        (n->link).left = (l == NULL) ? NULL : &(l->link);               \
    }                                                                   \
    static inline void set_ri(TNode_##name* n, TNode_##name *r) {       \
        (n->link).right = (r == NULL) ? NULL : &(r->link);              \
    }                                                                   \
                                                                        \
    static void init_node(TNode_##name* n, T t, TNode_##name* l, TNode_##name* r) { \
        n->t = t;                                                       \
        set_le(n, l);                                                   \
        set_ri(n, r);                                                   \
    }                                                                   \
                                                                        \
                                                                        \
                                                                        \
    bool name##_empty() {                                               \
        return left(name##_head) == NULL &&                             \
            right(name##_head) == NULL;                                 \
    }                                                                   \
                                                                        \
    static TNode_##name* find_fa(T t) {                                 \
        if (name##_empty()) {                                           \
            return NULL;                                                \
        }                                                               \
        TNode_##name *fa = name##_head;                                 \
        TNode_##name *le = left(name##_head);                           \
        assert(le != NULL);                                             \
        int a = f(le->t, t);                                            \
        while(a != 0) {                                                 \
            fa = le;                                                    \
            le = (a > 0) ? left(le) : right(le);                        \
            if(le == NULL) {                                            \
                return fa;                                              \
            }                                                           \
            a = f(le->t, t);                                            \
        }                                                               \
        return fa;                                                      \
    }                                                                   \
                                                                        \
    static TNode_##name* find_leaf_by(TNode_##name* n, T t) {           \
        assert(n != NULL);                                              \
        TNode_##name *fa = n;                                           \
        int a = f(fa->t, t);                                            \
        TNode_##name* son = (a > 0) ? left(fa) : right (fa);            \
        while(son != NULL) {                                            \
            fa = son;                                                   \
            a = f(son->t, t);                                           \
            son = (a > 0) ? left(fa) : right(fa);                       \
            if(son == NULL) {                                           \
                return fa;                                              \
            }                                                           \
        }                                                               \
        return fa;                                                      \
    }                                                                   \
                                                                        \
    static TNode_##name* find_leaf(T t) {                               \
        if (name##_empty()) {                                           \
            return name##_head;                                         \
        }                                                               \
        return find_leaf_by(left(name##_head), t);                      \
    }                                                                   \
                                                                        \
    void name##_add(T t) {                                              \
        TNode_##name *fa = find_leaf(t);                                \
        TNode_##name *now = kmalloc(sizeof (TNode_##name));             \
        init_node(now, t, NULL, NULL);                                  \
        if(fa == name##_head) {                                         \
            set_le(fa, now);                                            \
            return;                                                     \
        }                                                               \
        int a = f(fa->t, t);                                            \
        if (a <= 0) {                                                   \
            set_ri(fa, now);                                            \
        } else {                                                        \
            set_le(fa, now);                                            \
        }                                                               \
    }                                                                   \
                                                                        \
    bool name##_has(T t) {                                              \
        TNode_##name* fa = find_fa(t);                                  \
        if (fa == NULL) {                                               \
            return false;                                               \
        }                                                               \
        TNode_##name *le =  left(fa);                                   \
        TNode_##name *ri = right(fa);                                   \
        if ( (le != NULL && f(le->t, t) == 0)                           \
            || (ri != NULL && f(ri->t, t) == 0) ) {                     \
            return true;                                                \
        }                                                               \
        return false;                                                   \
    }                                                                   \
                                                                        \
    static int count_son(TNode_##name* n) {                             \
        NodeLink link = n->link;                                        \
        int count = 2;                                                  \
        if (link.left == NULL) {                                        \
            count--;                                                    \
        }                                                               \
        if(link.right == NULL) {                                        \
            count--;                                                    \
        }                                                               \
        return count;                                                   \
    }                                                                   \
                                                                        \
    static void find_replace(TNode_##name*, int);                       \
                                                                        \
    static void exchange_root(TNode_##name* node,                       \
        TNode_##name *(*get)(TNode_##name*),                            \
        void (*set)(TNode_##name*, TNode_##name*) ) {                   \
                                                                        \
        TNode_##name * son = get(node);                                 \
        assert(son != NULL);                                            \
        TNode_##name * l_big = find_leaf_by(son, node->t);              \
        assert(l_big != NULL);                                          \
        /*printk("*l_big->t %d, *node->t  %d\n", *l_big->t, *node->t);*/ \
        TNode_##name * fa = find_fa(l_big->t);                          \
        assert(fa != NULL);                                             \
        /* exchange root and a min/max node*/                           \
        T tmp = node->t;                                                \
        node->t = l_big->t;                                             \
        l_big->t = tmp;                                                 \
        int count = count_son(l_big);                                    \
        if (count == 0) {                                               \
            set(fa, NULL);                                              \
            kfree(l_big);                                               \
            return;\
        }                                                               \
        assert(count == 1);                                             \
        find_replace(l_big, count);                                     \
    }                                                                   \
                                                                        \
    static void find_replace(TNode_##name* aim, int count) {            \
        if (count == 1) {                                               \
            TNode_##name *le = left(aim);                               \
            TNode_##name *ri = right(aim);                              \
            if (le == NULL) {                                           \
                exchange_root(aim, right, set_ri);                      \
            } else {                                                    \
                assert(ri == NULL);                                     \
                exchange_root(aim, left, set_le);                       \
            }                                                           \
        } else {                                                        \
            assert(count == 2);                                         \
            exchange_root(aim, left, set_le);                           \
        }                                                               \
    }                                                                   \
                                                                        \
    bool name##_delete(T t) {                                           \
        if(!name##_has(t)) {                                            \
            printk("no such elements\n");                               \
            return false;                                               \
        }                                                               \
        TNode_##name* fa = find_fa(t);                                  \
        assert(fa != NULL);                                             \
        TNode_##name *le =  left(fa);                                   \
        TNode_##name *ri = right(fa);                                   \
        int count = 0;                                                  \
        bool left_son = false;                                          \
        if (le->t == t) {                                               \
            count = count_son(le);                                      \
            left_son = true;                                            \
        } else {                                                        \
            count = count_son(ri);                                      \
        }                                                               \
                                                                        \
        switch(count) {                                                 \
            case 0:                                                     \
                if (left_son) {                                         \
                    set_le(fa, NULL);                                   \
                    kfree(le);                                          \
                    return true;                                        \
                }                                                       \
                set_ri(fa, NULL);                                       \
                kfree(ri);                                              \
                break;                                                  \
            case 1:                                                     \
            case 2:                                                     \
                if(left_son) {                                          \
                    find_replace(le, count);                            \
                } else {                                                \
                    find_replace(ri, count);                            \
                }                                                       \
                break;                                                  \
            default:                                                    \
                assert(false);                                          \
                break;                                                  \
        }                                                               \
        return true;                                                    \
    }                                                                   \
                                                                        \
    
/*\
  bool empty_subtree(TNode_##name* root) {                     \
  assert(root != NULL);                                           \
  return left(root) == NULL                                       \
  && right(root) == NULL;                                     \
  }  */

#endif /* __BINARY_TREE_H__ */