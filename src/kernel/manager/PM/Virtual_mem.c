#include "kernel/manager/Virtual_mem.h"

/**
   This struct aims to store the virtual address information
   which vary from different processes.
   Including:
     code and read-only-data;
     bss
   Excluding:
     stack -- all user process is the same
     kernel space -- they share all spaces, so no need to store it.
 */

void vir_init(Seg_info *self, uint32_t start, uint32_t end, int flags) {
    self->start = start;
    self->end = end;
    self->flags = flags;
    list_init(&(self->link));
}
