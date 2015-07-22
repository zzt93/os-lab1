#include "kernel/memory.h"
#include "kernel/manager/MM.h"

uint32_t get_pa(CR3 *pdir, uint32_t va) {
    uint32_t pdir_idx = va >> 22;
    uint32_t ptable_idx = (va >> 12) & 0x3ff;
    uint32_t page_off = va & 0xfff;
    PDE *pde = (PDE *)((pdir->page_directory_base << 12) + pdir_idx);
    assert(pde->present == 1);
    PTE *pte = (PTE *)(page_frame_to_address(pde) + ptable_idx);
    assert(pte->present == 1);
    uint32_t page = page_frame_to_address(pte);
    assert((page & 0xfff) == 0);
    uint32_t pa = page | page_off;
    return pa;
}