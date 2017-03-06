#ifndef __MALLOC_H__
#define __MALLOC_H__


void init_kmalloc();

void *kmalloc(unsigned int size);

void kfree(void *);

const int *const allocate_start();

#define ALLOC_SIZE 262144 // 2**20 bytes = 2**18 words

#define ALIGNMENT 4 // the bytes to alignment
#define HEAD_SIZE 1 // use one word to store head info

#include "macro.h"

#define ALIG(size) _ALIGN_((size), (ALIGNMENT - 1))


typedef enum {
    M_IFADDR,
} EAllocType;

typedef enum {
    M_DONTWAIT,
    M_WAITOK,
} EMemoryAllocWait;

void *malloc(uint32_t size, EAllocType allocType, EMemoryAllocWait memoryAllocWait);


#endif /* __MALLOC_H__ */