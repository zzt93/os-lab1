#ifndef __MALLOC_H__
#define __MALLOC_H__


void *kmalloc(unsigned int size);
void kfree(void *);

void init_kmalloc();
const int * const allocate_start();

#define ALLOC_SIZE 262144 // 2**20 bytes = 2**18 words

#define ALIGNMENT 4 // the bytes to alignment
#define HEAD_SIZE 1 // use one word to store head info

#include "macro.h"

#define ALIG(size) _ALIGN_((size), (ALIGNMENT - 1))
/*
typedef struct {
    listhead link;
    int start;
    int size;
} mem;
*/
#endif /* __MALLOC_H__ */