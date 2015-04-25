#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "assert.h"

#define SIZE 512

#define QUEUE_DEF(T)                            \
    T queue[SIZE];                              \
    int tail = 0;                               \
    int head = 0;                               \


#define QUEUE_IMP(T)                            \
    bool enqueue(T t) {                         \
        if (tail > SIZE - 1) {                  \
            tail = tail % SIZE;                 \
        }                                       \
        queue[tail++] = t;                      \
        if (head == tail) {                     \
            return false;                       \
        }                                       \
        return true;                            \
    }                                           \
                                                \
    T dequeue() {                               \
        if (head > SIZE - 1) {                  \
            head = head % SIZE;                 \
        }                                       \
        int tmp = head++;                       \
        queue[tmp];                             \
    }                                           \


#undef SIZE
#endif /* __QUEUE_H__ */