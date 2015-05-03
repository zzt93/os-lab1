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


#define QUEUE(T, max, name) QUEUE_DEF(T, max + 1, name)

#define QUEUE_DEF(T, max, name)                 \
    static T queue[max];                        \
    static int tail = 0;                        \
    static int head = 0;                        \
                                                \
                                                \
    static void name##_enqueue(T t) {           \
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
    static T name##_dequeue() {                 \
        int tmp = head++;                       \
        if (head > max - 1) {                   \
            head = 0;                           \
        }                                       \
        return queue[tmp];                      \
    }                                           \
                                                \
    static bool name##_is_empty() {             \
        return head == tail;                    \
    }                                           \
                                                \
    T name##_pop_last() {                       \
        int tmp = --tail;                       \
        if (tmp < 0) {                          \
            tmp = max - 1;                      \
        }                                       \
        return queue[tmp];                      \
    }                                           \
                                                \
    static inline bool name##_size() {          \
        return tail - head;                     \
    }                                           \
                                                \
                                                \


#endif /* __QUEUE_H__ */