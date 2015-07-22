#include "x86/memory.h"

#include "kernel/semaphore.h"
#include "kernel/manager/PM_pdir.h"

static Pdir all_pdirs[PDIR_NUM] align_to_page;
static int f_pointer = 0;

/**
   TODO may change to bitmap to store usage info
   return: the physical address of page directory
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
    return (PDE*)va_to_pa(pdir);
}

void init_kernel_image(PDE* pdir) {
    // fill page directory using physical address
    PTE* ptable = (PTE*)va_to_pa(get_kptable());
    uint32_t pdir_idx;
    for (pdir_idx = 0; pdir_idx < KMEM / PD_SIZE; pdir_idx++) {
        make_specific_pde(&pdir[pdir_idx + KERNEL_VA_START / PD_SIZE], ptable, KERNEL_PAGE_ENTRY, PAGE_R);
        ptable += NR_PTE;
    }

}
