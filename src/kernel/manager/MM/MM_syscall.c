#include "kernel/manager/manager.h"
#include "kernel/message.h"

#include "kernel/process.h"

#include "kernel/memory.h"

#include "lib/string.h"

int page_copy(Msg *m) {
    PCB* src = (PCB*)m->i[0];
    PCB* dest = (PCB*)m->i[1];
    // allocate page directory
    PDE *pdir = pdir_alloc();
    set_pdir(dest, (uint32_t)pdir);
    PDE *s_p = (PDE*)get_pdir_addr(src);
    assert( ((int)s_p&0xfff) == 0);
    // copy page table and page
    PTE *ptable = NULL, *s_pt = NULL;
    Page *page = NULL;

    uint32_t pdir_i, ptable_idx;
    // except kernel image, including user stack
    for (pdir_i = 0; pdir_i < (KERNEL_VA_START >> 22); pdir_i++) {
        // if source process has a page directory entry,
        //ie a page table
        if (s_p[pdir_i].present == 1) {
            ptable = alloc_page();
            assert((((int)ptable & 0xfff) == 0) && ptable != NULL);
            make_pde(&pdir[pdir_i], ptable);

            s_pt = (PTE*)(s_p[pdir_i].page_frame << 12);
            for (ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx++) {
                if (s_pt[ptable_idx].present == 1) { // present
                    if (s_pt[ptable_idx].read_write == PAGE_W) {// read and write
                        page = alloc_page();
                        assert((((int)page & 0xfff) == 0) && page != NULL);
                        // make page table entry using the address of new page,
                        // and the flags of father
                        make_specific_pte(
                            &ptable[ptable_idx], page,
                            s_pt[ptable_idx].user_supervisor, s_pt[ptable_idx].read_write);
                        // copy the content in page
                        memcpy(page, (void *)(s_pt[ptable_idx].page_frame << 12), PAGE_SIZE);
                    } else {
                        assert(s_pt[ptable_idx].read_write == PAGE_R);
                        // TODO if s_pt[ptable_idx].read_write == PAGE_R, ie
                        // it is read only, just point to it.
                        make_specific_pte(
                            &ptable[ptable_idx], (void *)(s_pt[ptable_idx].page_frame << 12),
                            s_pt[ptable_idx].user_supervisor, s_pt[ptable_idx].read_write);
                    }
                }
            }
        }
    }

    // reply message
    return 1;
}

/**
   Copy `page directory, page table, page` from source process
   to destination process by the information stored in the vir_mem
   and user stack
void copy_page_by_vir(Msg* m) {
    PCB* src = (PCB*)m->i[0];
    PCB* dest = (PCB*)m->i[1];
    // allocate page directory
    PDE *pdir = pdir_alloc();
    set_pdir(dest, (uint32_t)pdir);
    PDE *s_p = (PDE*)get_pdir_addr(src);
    assert( ((int)s_p&0xfff) == 0);
    // copy page table and page
    PTE *ptable = NULL, *s_pt = NULL;
    Page *page = NULL;

    uint32_t pdir_i, ptable_idx;
    //for each vir_mem, allocate page directory, page table, page

    // for user stack
    // reply message
    return 1;
}
*/

/**
   In the perspective of program segment
 */
int seg_free(PCB *aim, Seg_info *v) {
    uint32_t low_b = ALIGN_PAGE_SZ(v->start) - PAGE_SIZE;
    assert((low_b & 0xfff) == 0);// ALIGN check
    assert((low_b & ~0xfff) == (v->start & ~0xfff));
    // free page
    // @checked corner case
    while (low_b < v->end) {
        free_page(get_pa(&aim->pdir, low_b));
        low_b += PAGE_SIZE;
    }
    // free page table
    uint32_t pt_low = ALIGN_PTABLE_SZ(v->start) - PT_SIZE;
    assert((pt_low & 0x3fffff));
    while (pt_low < v->end) {
        free_page(get_ptable(&aim->pdir, pt_low));
        pt_low += PT_SIZE;
    }
    return 1;
}

/**
   In the perspective of memory management
   free page, page table using stored virtual space information
 */
int page_free(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    ListHead *p = NULL;
    Seg_info *v = NULL;
    int state = 1;
    list_foreach(p, &(aim->vir_mem)) {
        v = list_entry(p, Seg_info, link);
        state &= seg_free(aim, v);
    }
    return state;
}