#ifndef __VIRTUAL_MEM_H__
#define __VIRTUAL_MEM_H__

#include "adt/list.h"

// store virtual address space [start, end) with flags
// flags: RWE is the lowest three bits
typedef struct {
    ListHead link;
    uint32_t start;
    uint32_t end;
    int flags;
} Vir_mem;

void vir_init(Vir_mem *self, uint32_t start, uint32_t end, int flags);
void add_bef(ListHead *, Vir_mem *);
void add_after(ListHead *, Vir_mem *);

#endif /* __VIRTUAL_MEM_H__ */