#ifndef __MALLOC_H__
#define __MALLOC_H__


void *kmalloc(unsigned int size);
void kfree(void *);

void init_kmalloc();

#define ALLOC_SIZE 1048576 // 2**20

#define ALIGNMENT 4 // the bytes to alignment
#define HEAD_SIZE (sizeof (int))

#define ALIG(size) _ALIGN_((size), (size - 1))
#define _ALIGN_(x, a) (((x) + a) & ~(a))
/*
typedef struct {
    listhead link;
    int start;
    int size;
} mem;
*/
#endif /* __MALLOC_H__ */