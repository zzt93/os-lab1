#ifndef __MALLOC_H__
#define __MALLOC_H__


void *kmalloc(unsigned int size);
void kfree(void *);

void init_kmalloc();

#define ALLOC_SIZE 262144 // 2**20 bytes = 2**18 words

#define ALIGNMENT 4 // the bytes to alignment
#define HEAD_SIZE 1 // use one word to store head info

#define ALIG(size) _ALIGN_((size), (ALIGNMENT - 1))
#define _ALIGN_(x, a) (((x) + (a)) & ~(a))
/*
typedef struct {
    listhead link;
    int start;
    int size;
} mem;
*/
#endif /* __MALLOC_H__ */