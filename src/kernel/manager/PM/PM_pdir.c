#include "x86/memory.h"

#include "kernel/semaphore.h"
#include "kernel/PM_pdir.h"

static Pdir all_pdirs[PDIR_NUM] align_to_page;
static int f_pointer = 0;

/**
   TODO may change to bitmap to store usage info
 */
PDE* pdir_alloc() {
    assert(f_pointer < PDIR_NUM);
    lock();
    Pdir* free = &all_pdirs[f_pointer];
    f_pointer++;
    unlock();
    // make it invalid at first
    PDE* pdir = (PDE*)free;
    int i;
    for (i = 0; i < NR_PDE; i++) {
        make_invalid_pde(&pdir[i]);
    }
    return pdir;
}

void init_kernel_image(PDE* pdir) {
    PTE* ptable = get_kptable();
    uint32_t pdir_idx;
    for (pdir_idx = 0; pdir_idx < KMEM / PD_SIZE; pdir_idx++) {
        make_pde(&pdir[pdir_idx + KERNEL_VA_START / PD_SIZE], ptable);
        ptable += NR_PTE;
    }

}
