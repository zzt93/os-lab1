#ifndef __HEAP_H__
#define __HEAP_H__

/**
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
    static int father(int i) {                  \
        int left = i % 2;                       \
        return left ? i / 2 : i / 2 - 1;        \
    }                                           \
                                                \
    static int l_child(int i) {                 \
        return i * 2 + 1;                       \
    }                                           \
                                                \
    static int r_child(int i) {                 \
        return i * 2 + 2;                       \
    }                                           \
                                                \
    /*return next index for this function*/     \
    /*for percolate down is left/right child*/  \
    static int cmp_swap(int i1) {               \
        unsigned int i2 = l_child(i1);            \
        if (i2+1 < size) {                      \
            if(f(heap[i2+1], heap[i2]) > 0){    \
                ++i2;                           \
            }                                   \
        }                                       \
        if (f(heap[i1], heap[i2]) < 0){         \
            T temp = heap[i1];                  \
            heap[i1] = heap[i2];                \
            heap[i2] = temp;                    \
        }                                       \
        return i2;                              \
    }                                           \
                                                \
    static void percolate_up(int i) {           \
        int tei = 0;                            \
        for (; i != 0 && i/2 >= 0; i/=2) {      \
            tei = i/2;                          \
            if (i%2 == 0 && tei-1 >= 0) {       \
                tei -= 1;                       \
            }                                   \
            cmp_swap(tei);                      \
        }                                       \
    }                                           \
                                                \
    static void percolate_down(int i) {         \
        for (; 2*i+1 < size; ) {                \
            i = cmp_swap(i);                    \
        }                                       \
    }                                           \
                                                \
    static T max() {                            \
        return heap[0];                         \
    }                                           \
                                                \
    static T pop_max() {                        \
        T t = heap[0];                          \
        heap[0] = heap[--size];                 \
        percolate_down(0);                      \
        return t;                               \
    }                                           \
                                                \
    static void add(T t) {                      \
        heap[size++] = t;                       \
        percolate_up(size - 1);                 \
    }                                           \



#endif /* __HEAP_H__ */