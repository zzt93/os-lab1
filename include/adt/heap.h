#ifndef __HEAP_H__
#define __HEAP_H__

#include "kernel/semaphore.h"

/**
   @see docs/ADT/heap.md for latest version
   Usage:
   HEAP(T, capacity, f)
   T -- type
   capacity -- the largest size of this heap
   f(T*, T*) -- function to compare elements, can be used to
   adjust max-heap or min-heap

    - Invariant:
        f(father, left_child) >= 0 && f(father, right_child) >= 0

   Internal:
   Using array to implement heap, ie priority queue

   array start from 0,
   so if father index is `i`
   left child is `2 * i + 1`
   right child is `2 * i + 2`
 */


#define HEAP(T, capacity, f)                    \
    static T heap[capacity];                    \
    static int size = 0;                        \
                                                \
    static inline int father(int i) {           \
        int left = i % 2;                       \
        return left ? i / 2 : i / 2 - 1;        \
    }                                           \
                                                \
    static inline int l_child(int i) {          \
        return i * 2 + 1;                       \
    }                                           \
                                                \
    /*return next index for this function*/     \
    /*just for percolate_down()*/               \
    /*is left/right child index*/               \
    static int cmp_swap(int i1) {               \
        unsigned int i2 = l_child(i1);          \
        if (i2+1 < size) {                      \
            if(f(heap + i2+1, heap + i2) > 0){  \
                ++i2;                           \
            }                                   \
        }                                       \
        if (f(heap + i1, heap + i2) < 0){       \
            T temp = heap[i1];                  \
            heap[i1] = heap[i2];                \
            heap[i2] = temp;                    \
        }                                       \
        return i2;                              \
    }                                           \
                                                \
    static void percolate_up(int i) {           \
        int fa = 0;                             \
        for (; i != 0 && i/2 >= 0; i = fa) {    \
            fa = father(i);                     \
            cmp_swap(fa);                       \
        }                                       \
    }                                           \
                                                \
    static void percolate_down(int i) {         \
        while (l_child(i) < size) {             \
            i = cmp_swap(i);                    \
        }                                       \
    }                                           \
                                                \
    static inline int empty() {                 \
        return size == 0;                       \
    }                                           \
                                                \
    static T max() {                            \
        if (empty()) {                          \
            assert(0);                          \
        }                                       \
        return heap[0];                         \
    }                                           \
                                                \
    static T pop_max() {                        \
        if (empty()) {                          \
            assert(0);                          \
        }                                       \
        lock();                                 \
        T t = heap[0];                          \
        heap[0] = heap[--size];                 \
        unlock();                               \
        percolate_down(0);                      \
        return t;                               \
    }                                           \
                                                \
    static void add(T t) {                      \
        lock();                                 \
        heap[size++] = t;                       \
        unlock();                               \
        percolate_up(size - 1);                 \
    }                                           \

#define heap_each(i, t)                                     \
    for (i = 0, t = heap + i; i < size; i++, t = heap + i)

/*
                                                  \
    static int r_child(int i) {                 \
        return i * 2 + 2;                       \
    }                                           \

 */

#endif /* __HEAP_H__ */