#include "kernel/manager/Virtual_mem.h"

/**
   This struct aims to store the virtual address information
   which vary from different processes.
   Including:
     code and read-only-data;
     bss
   Excluding:
     stack -- all user process is the same
     kernel thread -- they share all spaces, so no need to store it.
 */

void vir_init(Vir_mem *self, uint32_t start, uint32_t end, int flags) {
    self->start = start;
    self->end = end;
    self->flags = flags;
    list_init(&(self->link));
}

void add_bef(ListHead *link, Vir_mem *v) {
}

void add_after(ListHead *link, Vir_mem *v) {
}