#ifndef __BIT_MAP_H__
#define __BIT_MAP_H__

/**
   Usage:
   - declare:
   BIT_MAP(size)
   - method:
   set_val(i, val) -- set position `i` to be `val`
   is_val(i, val) -- test whether `i` is `val`
   first_val(val) -- return the first position where value if `val`


   This container is not thread safe
 */

#include "kernel/semaphore.h"

#define BITS 32
#define FREE 0
#define USED 1
#define INVALID -1

#define BIT_MAP(size)                               \
    static uint32_t free[size / BITS + 1];          \
                                                    \
    static void set_val(int i, int val) {           \
        assert(val == FREE || val == USED);         \
        assert(i >= 0);                             \
        int index = i / BITS;                       \
        int j = i % BITS;                           \
        int bit = val << j;                         \
        assert(bit == (val << i));                  \
        /* if upper assert fail change all << i*/   \
        assert((free[index]&(1 << j)) != bit);      \
        /* set that bit to val*/                    \
        free[index] ^= (-val ^ free[index])         \
            & (1 << j);                             \
    }                                               \
                                                    \
    static int is_val(int i, int val) {             \
        assert(i >= 0);                             \
        int index = i / BITS;                       \
        int j = i % BITS;                           \
        int res = free[index] >> j;                 \
        return (res & 1) == val;                    \
    }                                               \
                                                    \
    static int first_val(int val) {                 \
        int i;                                      \
        for (i = 0; i < size; i++) {                \
            if (is_val(i, val)) {                   \
                return i;                           \
            }                                       \
        }                                           \
        return INVALID;                             \
    }                                               \




#endif /* __BIT_MAP_H__ */