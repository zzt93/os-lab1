#ifndef __HEAP_H__
#define __HEAP_H__

#include "kernel/semaphore.h"

/**
   @see docs/ADT/heap.md for latest version
   Usage:
   HEAP(T, capacity, f)
   T -- type, might as well be a pointer type
   capacity -- the largest size of this heap
   f(T, T) -- function to compare elements, can be used to
   adjust max-heap or min-heap

    - Invariant:
        f(father, left_child) >= 0 && f(father, right_child) >= 0

   Internal:
   Using array to implement heap, i.e. priority queue

   array start from 0,
   so if father index is `i`
   left child is `2 * i + 1`
   right child is `2 * i + 2`
 */


#define HEAP(T, capacity, f, name)                  \
    static T _heap[capacity];                       \
    static int _heap_size = 0;                      \
                                                    \
    static inline int father(int i) {               \
        int left = i % 2;                           \
        return left ? i / 2 : i / 2 - 1;            \
    }                                               \
                                                    \
    static inline int l_child(int i) {              \
        return i * 2 + 1;                           \
    }                                               \
                                                    \
    /*return next index for this function*/         \
    /*just for percolate_down()*/                   \
    /*is left/right child index*/                   \
    static int cmp_swap(int i1) {                   \
        unsigned int i2 = l_child(i1);              \
        if (i2+1 < _heap_size) {                    \
            if(f(_heap[i2+1], _heap[i2]) > 0){      \
                ++i2;                               \
            }                                       \
        }                                           \
        if (f(_heap[i1], _heap[i2]) < 0){           \
            T temp = _heap[i1];                     \
            _heap[i1] = _heap[i2];                  \
            _heap[i2] = temp;                       \
        }                                           \
        return i2;                                  \
    }                                               \
                                                    \
    static void percolate_up(int i) {               \
        int fa = 0;                                 \
        for (; i != 0 && i/2 >= 0; i = fa) {        \
            fa = father(i);                         \
            cmp_swap(fa);                           \
        }                                           \
    }                                               \
                                                    \
    static void percolate_down(int i) {             \
        while (l_child(i) < _heap_size) {           \
            i = cmp_swap(i);                        \
        }                                           \
    }                                               \
                                                    \
    static inline int name##_empty() {              \
        return _heap_size == 0;                     \
    }                                               \
                                                    \
    static inline int name##_heap_size() {          \
        return _heap_size;                          \
    }                                               \
                                                    \
    T name##_max() {                                \
        if (name##_empty()) {                       \
            assert(0);                              \
        }                                           \
        return _heap[0];                            \
    }                                               \
                                                    \
    T name##_pop_max() {                            \
        if (name##_empty()) {                       \
            assert(0);                              \
        }                                           \
        lock();                                     \
        T t = _heap[0];                             \
        _heap[0] = _heap[--_heap_size];             \
        unlock();                                   \
        percolate_down(0);                          \
        return t;                                   \
    }                                               \
                                                    \
    void name##_add(T t) {                          \
        lock();                                     \
        _heap[_heap_size++] = t;                    \
        unlock();                                   \
        percolate_up(_heap_size - 1);               \
    }                                               \
                                                    \
    void name##_update(int i, T t) {                \
        if (i >= _heap_size) {                      \
            return;                                 \
        }                                           \
        lock();                                     \
        int res = f(_heap[i], t);                   \
        _heap[i] = t;                               \
        if(res > 0) {                               \
            percolate_down(i);                      \
        } else if (res < 0){                        \
            percolate_up(i);                        \
        }                                           \
        unlock();                                   \
    }                                               \

#define heap_each(i, t)                                             \
    for (i = 0, t = _heap[i]; i < _heap_size; i++, t = _heap[i])

/*
                                                  \
    static int r_child(int i) {                 \
        return i * 2 + 2;                       \
    }                                           \

 */

#endif /* __HEAP_H__ */