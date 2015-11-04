#ifndef __D_BIT_MAP_H__
#define __D_BIT_MAP_H__

/**
   Usage:
   - declare:
   D_BIT_MAP() -- a dynamic allocated version of bitmap
   - method:
   set_val(i, val) -- set position `i` to be `val`
   i -- start from 0
   is_val(i, val) -- test whether `i` is `val`
   i -- start from 0
   first_val(val) -- return the first position where value is `val`


   This container is not thread safe
*/

#include "kernel/semaphore.h"
#include "lib/malloc.h"
#include "lib/string.h"

#define BITS 8
#define FREE 0
#define USED 1
#define INVALID -1

#define D_BIT_MAP()                                             \
    static uint8_t *_bitmap_array;                              \
    static int _bitmap_size;                                    \
                                                                \
    static void init_bitmap(int size) {                         \
        assert(size > 0);                                       \
        int actual_size = size % BITS == 0 ?                    \
            size / BITS : size / BITS + 1;                      \
        _bitmap_array = kmalloc(actual_size);                   \
        _bitmap_size = size;                                    \
        memset(_bitmap_array, 0, actual_size);                  \
    }                                                           \
                                                                \
    static inline uint8_t * bits() {                            \
        return _bitmap_array;                                   \
    }                                                           \
                                                                \
    static int set_val(int i, uint8_t val) {                    \
        assert(val == FREE || val == USED);                     \
        assert(i >= 0 && i < _bitmap_size);                     \
        int index = i / BITS;                                   \
        int j = i % BITS;                                       \
        uint8_t bit = val << j;                                 \
        assert(bit == (val << i%BITS));                         \
        /* if upper assert fail change all << i*/               \
        assert((_bitmap_array[index]&(1 << j)) != bit);         \
        /* set that bit to val*/                                \
        _bitmap_array[index] ^= (-val ^ _bitmap_array[index])   \
            & (1 << j);                                         \
        return index;                                           \
    }                                                           \
                                                                \
    static int is_val(int i, int val) {                         \
        assert(i >= 0 && i < _bitmap_size);                     \
        int index = i / BITS;                                   \
        int j = i % BITS;                                       \
        int res = _bitmap_array[index] >> j;                    \
        return (res & 1) == val;                                \
    }                                                           \
                                                                \
    static int first_val(int val) {                             \
        int i;                                                  \
        for (i = 0; i < _bitmap_size; i++) {                    \
            if (is_val(i, val)) {                               \
                return i;                                       \
            }                                                   \
        }                                                       \
        return INVALID;                                         \
    }                                                           \



#endif /* __D_BIT_MAP_H__ */