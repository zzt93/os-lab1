#include "kernel/memory.h"
#include "kernel/manager/MM.h"

PTE * get_ptable(CR3 *pdir, uint32_t va) {
    uint32_t pdir_idx = va >> 22;
    uint32_t ptable_idx = (va >> 12) & 0x3ff;
    PDE *pde = (PDE *)(pdir->page_directory_base << 12) + pdir_idx;
    assert(pde->present == 1);
    PTE *pte = (PTE *)page_frame_to_address(pde) + ptable_idx;
    assert(pte->present == 1);
    return pte;
}

void * get_pa(CR3 *pdir, uint32_t va) {
    uint32_t page_off = va & 0xfff;
    PTE *pte = get_ptable(pdir, va);
    assert(pte->present == 1);
    uint32_t page = page_frame_to_address(pte);
    assert((page & 0xfff) == 0);
    uint32_t pa = page | page_off;
    if (pdir == get_kcr3()) {
        assert(pa == va);
        return (void *)va;
    }
    return (void *)pa;
}
