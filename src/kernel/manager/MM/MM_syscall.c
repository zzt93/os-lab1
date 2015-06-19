#include "kernel/manager/manager.h"
#include "kernel/message.h"

#include "kernel/process.h"

#include "kernel/memory.h"

void page_copy(Msg *m) {
    PCB* src = (PCB*)m->i[0];
    PCB* dest = (PCB*)m->i[1];
    pid_t aim = m->src;
    // allocate page directory
    PDE *pdir = (PDE*)va_to_pa(pdir_alloc());
    set_pdir(dest, (uint32_t)pdir);
    PDE *s_p = (PDE*)(src->pdir.page_directory_base << 12);
    assert( ((int)s_p&0xfff) == 0);
    // copy page table and page
    PTE *ptable = NULL, *s_pt = NULL;
    Page *page = NULL;

    uint32_t pdir_i, ptable_idx;
    for (pdir_i = 0; pdir_i < NR_PDE; pdir_i++) {
        if (s_p[pdir_i].present == 1) {
            ptable = alloc_page();
            assert((((int)ptable & 0xfff) == 0) && ptable != NULL);
            make_pde(&pdir[pdir_i], ptable);

            s_pt = (PTE*)(s_p[pdir_i].page_frame << 12);
            for (ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx++) {
                if (s_pt[ptable_idx].present == 1) {
                    page = alloc_page();
                    assert((((int)page & 0xfff) == 0) && page != NULL);
                    // make page table entry using the address of new page,
                    // and the flags of father
                    make_specific_pte(
                        &ptable[ptable_idx], page,
                        s_pt[ptable_idx].user_supervisor, s_pt[ptable_idx].read_write);
                }

            }

        }
    }

    // reply message
    m->ret = 1;
    m->src = current->pid;
    send(aim, m);
}

