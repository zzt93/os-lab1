#include "kernel/manager/MM.h"
#include "kernel/message.h"
#include "kernel/process.h"

#include "kernel/memory.h"

extern PCB* current;

/**
   对于进程需要用到的每一个虚拟地址va, 我们来模拟地址转换的过程,
   如果发现对应的页目录项无效, 那么就为这个页目录项分配一张页表,
   并填写页目录项;
   如果发现对应的页表项无效, 那么就为这个页表项分配一个物理页面,
   并填写页表项.
   FM需要把程序加载到这些物理页面中,
   使得将来用户进程运行的时候, 能够通过我们填写的页目录和页表, 找到一些物理页面, 同时这些物理页面中正好包含着程序的内容, 这样用户进程才能正确运行, 否则将会抛出异常.
   填写页目录项和页表项分别使用make_pde()函数和make_pte()函数, 它们都在src/kernel/memory/util.c中定义, 你可以在src/kernel/memory/kvm.c中参考内核启动分页机制的过程.
   内核映像占用0xc0000000及以上的虚拟地址, 我们只需要让相应的页目录项指向相应的内核页表就可以了, 无须为内核映像重新分配页表.

   allocate new virtual page to a process
 */
int init_va(Msg* m) {
    unsigned int off = (unsigned int)m->offset;
    //assert(m->i[1] == m->dev_id);
    assert(off >= 0 && off < KERNEL_VA_START);
    assert(m->len > 0);
    assert(((int)m->buf & 0xfff) == 0);

    int US = m->i[1] >> 2;
    int RW = (m->i[1] >> 1) & 0x1;
    int count = 0;

    PDE *pdir = (PDE*)m->buf;
    PTE *ptable = NULL;
    Page *page = NULL;

	uint32_t pdir_idx, ptable_idx;
    //pdir_idx = off >> 22;
    //ptable_idx = (off >> 12) & 0x3ff;
    // TODO @see seg_free to simplify this loop
    for (; off < m->offset + m->len; off++) {
        pdir_idx = off >> 22;
        ptable_idx = (off >> 12) & 0x3ff;
        if (pdir[pdir_idx].present == 0) {
            ptable = alloc_page();
            assert((((int)ptable & 0xfff) == 0) && ptable != NULL);
            make_pde(&pdir[pdir_idx], ptable);
        } else {
            ptable = (PTE*)(pdir[pdir_idx].page_frame << 12);
        }
        if (ptable[ptable_idx].present == 0) {
            page = alloc_page();
            count++;
            if (count == 1) {
                m->buf = page;
            }
            assert((((int)page & 0xfff) == 0) && page != NULL);
            make_specific_pte(&ptable[ptable_idx], page, US, RW);
        }
    }

    // send back with the starting physical address
    // which is set in the upper loop
    return 1;
}
