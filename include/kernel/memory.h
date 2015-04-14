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
void make_pte(PTE *, void *);

#define va_to_pa(addr) \
	((void*)(((uint32_t)(addr)) - KOFFSET))

#define pa_to_va(addr) \
	((void*)(((uint32_t)(addr)) + KOFFSET))

/* the maxinum kernel size is 16MB */
#define KMEM    (16 * 1024 * 1024)

/* Nanos has 128MB physical memory  */
#define PHY_MEM   (128 * 1024 * 1024)

#endif
