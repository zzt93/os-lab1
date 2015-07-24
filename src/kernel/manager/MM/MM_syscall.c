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
    PDE *s_p = (PDE*)(src->pdir.page_directory_base << 12);
    assert( ((int)s_p&0xfff) == 0);
    // copy page table and page
    PTE *ptable = NULL, *s_pt = NULL;
    Page *page = NULL;

    uint32_t pdir_i, ptable_idx;
    for (pdir_i = 0; pdir_i < NR_PDE; pdir_i++) {
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
void copy_page_by_vir(Msg* m) {
    PCB* src = (PCB*)m->i[0];
    PCB* dest = (PCB*)m->i[1];
    // allocate page directory
    PDE *pdir = pdir_alloc();
    set_pdir(dest, (uint32_t)pdir);
    PDE *s_p = (PDE*)(src->pdir.page_directory_base << 12);
    assert( ((int)s_p&0xfff) == 0);
    // copy page table and page
    PTE *ptable = NULL, *s_pt = NULL;
    Page *page = NULL;

    uint32_t pdir_i, ptable_idx;
    //for each vir_mem, allocate page directory, page table, page

    // reply message
    return 1;
}
*/


int page_free(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    ListHead *p = NULL;
    Vir_mem *v = NULL;
    list_foreach(p, &(aim->vir_mem)) {
        v = list_entry(p, Vir_mem, link);
        
    }
    return 1;
}