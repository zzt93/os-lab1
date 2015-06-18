#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "x86/x86.h"

#define KOFFSET 0xC0000000

inline CR3* get_kcr3();
inline PDE* get_kpdir();
inline PTE* get_kptable();

void make_invalid_pde(PDE *);
void make_invalid_pte(PTE *);
void make_pde(PDE *, void *);
void make_specific_pde(PDE *, void *, int us, int rw);
void make_pte(PTE *, void *);
void make_specific_pte(PTE *, void *, int us, int rw);

void set_tss_esp0(uint32_t);

#define va_to_pa(addr) \
	((void*)(((uint32_t)(addr)) - KOFFSET))

#define pa_to_va(addr) \
	((void*)(((uint32_t)(addr)) + KOFFSET))

/* the maxinum kernel size is 16MB */
#define KMEM    (16 * 1024 * 1024)

/* Nanos has 128MB physical memory  */
#define PHY_MEM   (128 * 1024 * 1024)

// kernel starting physical address
#define KERNEL_PA_START 0
// kernel ending physical address
#define KERNEL_PA_END KMEM
// kernel starting virtual memory
#define KERNEL_VA_START KOFFSET
// user available pages
#define USER_FREE_PAGES ((PHY_MEM - KMEM) / PAGE_SIZE)

#define USER_STACK_BASE KOFFSET
#define USER_STACK_POINTER (USER_STACK_BASE - PAGE_SIZE)
#define USER_STACK_SIZE PAGE_SIZE

#define USER_PAGE_ENTRY 1
#define KERNEL_PAGE_ENTRY 0
#define PAGE_W 1
#define PAGE_R 0


#endif
