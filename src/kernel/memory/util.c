#include "x86/memory.h"

void
make_invalid_pde(PDE *p) {
	p->val = 0;
}
void
make_invalid_pte(PTE *p) {
	p->val = 0;
}

/* For simplicity, we make all pages readable and writable for all ring 3 processes.
 * In Lab3, you may set different flags for different pages to perform the whole
 * page level protection. */

void
make_specific_pde(PDE *p, void *addr, int us, int rw) {
	p->val = 0;
	p->page_frame = ((uint32_t)addr) >> 12;
	p->present = 1;
	p->read_write = rw;
	p->user_supervisor = us;
}

void
make_specific_pte(PTE *p, void *addr, int us, int rw) {
	p->val = 0;
	p->page_frame = ((uint32_t)addr) >> 12;
	p->present = 1;
	p->read_write = rw;
	p->user_supervisor = us;
}


void make_pde(PDE *p, void *addr) {
    make_specific_pde(p, addr, 1, 1);
}

void make_pte(PTE *p, void *addr) {
    make_specific_pte(p, addr, 1, 1);
}

