#ifndef __BIT_MAP_H__
#define __BIT_MAP_H__

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
        assert(i >= 0 && i < USER_FREE_PAGES);      \
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
        assert(i >= 0 && i < USER_FREE_PAGES);      \
        int index = i / BITS;                       \
        int j = i % BITS;                           \
        lock();                                     \
        int res = free[index] >> j;                 \
        unlock();                                   \
        return (res & 1) == val;                    \
    }                                               \
                                                    \
    static int first_val(int val) {                 \
        int i;                                      \
        for (i = 0; i < size; i++) {                \
            if (is_val(i, FREE)) {                  \
                return i;                           \
            }                                       \
        }                                           \
        return INVALID;                             \
    }                                               \




#endif /* __BIT_MAP_H__ */