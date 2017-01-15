#include "lib/malloc.h"
#include "kernel/semaphore.h"
#include "assert.h"
#include "lib/printk.h"

static int space[ALLOC_SIZE];
// the index of last free space, words
int last_i = 0;

void check_header(const int *h);

static int *space_head() {
    return space;
}

const int * const allocate_start() {
    return space;
}

void init_kmalloc() {
    int *h = space_head();
    *h = ALLOC_SIZE - HEAD_SIZE;
}



/**
   use lock & unlock rather than
   P & V -- have a try

  args: {size: use the sizeof to produce numbers of bytes}
  return value: if has free space, return the pointer to the first char; otherwise return null
  Usage: return the a memory block which has `size` bytes

  use the first sign bit as indicator of use or not
       1 as use
       0 as free
       ------------------------------------
       1/0|xxxx...|1/0|xxxx....
       ------------------------------------

       the head store the numbers of words(32bits)

*/
void *kmalloc(unsigned int size) {
    assert (ALIG(size + 4 * HEAD_SIZE)%4 == 0);
    unsigned int s = ALIG(size + 4 * HEAD_SIZE) / 4;
    // return null if the size is to large
    if (s >= ALLOC_SIZE) {
        return NULL;
    }

    lock();
    int *h = last_i + space_head();
    check_header(h);
    int *nextH = h + (*h > 0 ? *h : -*h);
    if (*h > 0 &&
        nextH < space_head() + ALLOC_SIZE && *nextH > 0) {
        *h = *h + *nextH;
    }
    check_header(h);


    int count = 2;
    while (count) {
        while (h < space_head() + ALLOC_SIZE && // valid address
            (*h < 0 || (*h > 0 && *h < s)) ) {
            //printk("in loop\n");
            if (*h < 0) {
                h += -(*h);
            } else if (*h > 0) {
                h += *h;
            } else {
                assert(0);
            }
        }
        if (h >= space_head() + ALLOC_SIZE) {
            h = space_head();
        } else { // *h >= s
            break;
        }
        count--;
    }
    // return null if we can't find a place for it to use
    if (count == 0) {
        panic("Nowhere to allocate it %l", size);
    }

    assert(s > 0);
    assert(*h >= s);
    printk("head is at %x, apply for %d words, before allocating have %x\n", h, s, *h);
    // write header info -- use int*
    int free = *h;
    int gap = h - space_head();
    *h = -(s);
    assert(free >= s);
    // update last_i
    last_i = gap + s;
    if (free != s) {
        // update free space header info if
        // we won't write a `0` to it. i.e.
        // we run out of a block memory in middle
        /**
           eg. s = 4
          ----------------------------------------------
          |   used1  | free = 4 | used2  | free = ...  |
          ----------------------------------------------
                     ^
                   old last_i
         */
        *(space_head() + last_i) = free - s;
    }

    NOINTR;
    unlock();
    return h + HEAD_SIZE;
}

void check_header(const int *h) {
    if (*h >= ALLOC_SIZE || *h == 0 || *h <= -ALLOC_SIZE) {
        panic("malloc header overwritten! allocate too few or write too much");
    }
}

void kfree(void *p) {
    int *h = (int *)p - HEAD_SIZE;
    if(*h > 0 || *h <= - ALLOC_SIZE) {
        assert(0);
    }
    lock();
    int *nextH = h + (-*h);

    int size = *h;
    printk("head is at %x, free %d words, ", h, size);
    if (*nextH <= 0) {
        // next is also used, so just release this is fine
        *h = -size;
    } else {
        // next is free, merge it
        *h = *nextH + (-size);
    }
    printk("after free have %x\n", *h);
    last_i = h - space_head();
    unlock();
}