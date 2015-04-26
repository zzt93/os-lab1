#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "assert.h"


/*
  {T: the type of queue}
  {m: the max size of your queue}
  return value:
  Usage:
     QUEUE(T, m)

     //then, use your queue
     if (!is_empty()) {
         enqueue();
         //.....
         dequeue();
     }

*/


#define QUEUE(T, max) QUEUE_DEF(T, max + 1)

#define QUEUE_DEF(T, max)                       \
    static T queue[max];                        \
    static int tail = 0;                        \
    static int head = 0;                        \
                                                \
                                                \
    static void enqueue(T t) {                  \
        queue[tail++] = t;                      \
        if (tail > max - 1) {                   \
            tail = 0;                           \
        }                                       \
        if (head == tail) {                     \
            printk("too many elements\n");      \
            assert(false);                      \
        }                                       \
    }                                           \
                                                \
    static T dequeue() {                        \
        int tmp = head++;                       \
        if (head > max - 1) {                   \
            head = 0;                           \
        }                                       \
        return queue[tmp];                      \
    }                                           \
                                                \
    static bool is_empty() {                    \
        return head == tail;                    \
    }                                           \


#endif /* __QUEUE_H__ */