#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "common.h"

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
            printk(str(name)"_queue: too many elements\n"); \
            assert(false);                      \
        }                                       \
    }                                           \
                                                \
    static bool name##_is_empty() {             \
        return head == tail;                    \
    }                                           \
                                                \
    static T name##_dequeue() {                 \
        if (name##_is_empty()) {                \
            printk(str(name)"_queue is empty"); \
            assert(false);                      \
        }                                       \
        int tmp = head++;                       \
        if (head > max - 1) {                   \
            head = 0;                           \
        }                                       \
        return queue[tmp];                      \
    }                                           \
                                                \
                                                \
    T name##_pop_last() {                       \
        if (name##_is_empty()) {                \
            printk(str(name)"_queue is empty"); \
            assert(false);                      \
        }                                       \
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