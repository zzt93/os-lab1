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
} Seg_info;

void vir_init(Seg_info *self, uint32_t start, uint32_t end, int flags);
void add_bef(ListHead *, Seg_info *);
void add_after(ListHead *, Seg_info *);

#endif /* __VIRTUAL_MEM_H__ */