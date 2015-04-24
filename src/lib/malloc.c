#include "lib/malloc.h"
#include "assert.h"
#include "lib/printk.h"

static char space[ALLOC_SIZE];
// the index of last free space
int last_i = 0;

static int *head() {
    return (int *)space;
}

void init_kmalloc() {
    int *h = head();
    *h = ALLOC_SIZE - HEAD_SIZE;
}



/*
  args: {size: use the sizeof to produce numbers of bytes}
  return value: if has free space, return the pointer to the first char; otherwise return null
  Usage:

  use the first sign bit as indicator of use or not
       1 as use
       0 as free
       ------------------------------------
       1/0|xxxx...|1/0|xxxx....
       ------------------------------------

*/
void *kmalloc(unsigned int size) {
    unsigned int s = ALIG(size + HEAD_SIZE);
    // return null if the size is to large
    if (s >= ALLOC_SIZE) {
        return NULL;
    }

    int *h = last_i + head();
    int count = 2;
    while (count) {
        while (h < head() + ALLOC_SIZE && *h < s) {
            if (*h <= 0) {
                h += -(*h);
            } else {
                h += *h;
            }
        }
        if (h >= head() + ALLOC_SIZE) {
            h = head();
        } else { // *h >= *s
            break;
        }
        count--;
    }
    // return null if can't find a place to place it
    if (count == 0) {
        return NULL;
    }

    printk("head is at %x, size is %d\n", h, s);
    // write header info
    int free = *h;
    int gap = h - head();
    *h = -(s);
    // update last_i
    last_i = gap + s;
    // update free space header info
    *(head() + last_i) = free - s;
    return h + HEAD_SIZE;
}

void kfree(void *p) {
    int *h = (int *)p - HEAD_SIZE;
    if(*h > 0 || *h <= - ALLOC_SIZE || *h%4 != 0) {
        assert(0);
    }
    int *nextH = h + *h;

    int size = *h;
    printk("head is at %x, size is %d\n", h, size);
    if (*nextH <= 0) {
        // next is also used, so just release this is fine
        *h = size;
    } else {
        // next is free, merge it
        *h = *nextH + size;
    }
    last_i = h - head();
}