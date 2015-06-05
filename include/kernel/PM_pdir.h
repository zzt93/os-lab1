#ifndef __PM_PDIR_H__
#define __PM_PDIR_H__

#include "memory.h"

typedef struct {
    PDE dir[NR_PDE];
} Pdir;

PDE* pdir_alloc();
void init_kernel_image(PDE*);

#define PDIR_MEM PAGE_SIZE
#define PDIR_NUM (KMEM / PDIR_MEM / 32)

#endif /* __PM_PDIR_H__ */