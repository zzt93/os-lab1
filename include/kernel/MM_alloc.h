#ifndef __MM_ALLOC_H__
#define __MM_ALLOC_H__

#include "common.h"
#include "memory.h"

typedef struct {
    uint8_t p[PAGE_SIZE];
} Page;

void* alloc_page();
void free_page(void* addr);

#endif /* __MM_ALLOC_H__ */