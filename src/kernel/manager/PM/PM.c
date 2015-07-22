#include "kernel/process.h"
#include "kernel/init_proc.h"
#include "kernel/message.h"

#include "kernel/manager/manager.h"

#include "kernel/memory.h"

#include "kernel/manager/PM_syscall.h"

#include "lib/malloc.h"
#define B_SIZE 512

extern PCB* current;

int PM;

/**
   The message sent to PM should specify:
   m->type
       @type:PM_CREATE
       m->i[0] -- the program/file name
       @return: The message send from PM specify:
       m->ret -- whether successfully create a user process

       @type:PM_fork
       m->buf -- the father's PCB's address
       @return:
       m->ret -- the child pid

       @type:PM_exec
       m->i[0] -- the program/file name
       m->buf -- address of string
       @return: 
*/
static void PM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        switch(m.type) {
            case PM_CREATE:
                create_process(&m);
                break;
            case PM_fork:
                kfork(&m);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void init_PM() {
    PCB* p = create_kthread(PM_job);
    PM = p->pid;
    add2wake(p);
}

// initialize the va for stack
// set the page directory, page table for user stack
// and allocate page
void create_va_stack(PDE* pdir, uint32_t *ss, uint32_t *esp) {
    unsigned char *va = (unsigned char*)USER_STACK_POINTER;
    assert(va == (unsigned char*)0xbffff000);
    Msg m;
    init_msg(&m,
        current->pid,
        NEW_PAGE,
        INVALID_ID, INVALID_ID, pdir, (int)va, USER_STACK_SIZE);

    send(MM, &m);
    receive(MM, &m);
    // TODO which segment, although all segment are
    // the same for time being
    *ss = SELECTOR_USER(SEG_USER_DATA);
    *esp = (uint32_t)(va + USER_STACK_SIZE);
    assert(*esp == KOFFSET);
}

/**
   whether to use this message to send
 */
void create_process(Msg* m) {
    int name = m->i[0];
    int dest = m->src;
    assert(name >= 0);
    char buf[B_SIZE];

    // create page directory for new process
    // must using physical address for cr3 should only store pa
    PDE* pdir = pdir_alloc();
    assert( ((int)pdir&0xfff) == 0);
    /* read 512 bytes starting from offset 0 from file "0" into buf */
	/* it contains the ELF header and program header table */
    init_msg(m,
        current->pid,
        FM_READ,
        INVALID_ID, name, buf, 0, B_SIZE);
	send(FM, m);
    receive(FM, m);

    struct ELFHeader *elf = (struct ELFHeader*)buf;
    struct ProgramHeader *ph_table, *end_ph;
    unsigned char *va, *pa, *i;
    //store virtual address space in loop
    ListHead vir_range;
    list_init(&vir_range);

	ph_table = (struct ProgramHeader*)((char *)elf + elf->phoff);
    // ignore the stack header for the time being
    end_ph = ph_table + elf->phnum - 1;
	for (; ph_table < end_ph; ph_table++) {
		/* scan the program header table, load each segment into memory */

		va = (unsigned char*)ph_table->vaddr; /* virtual address */
        //assert(va >= 0x08048000);
        assert(va > (unsigned char*)0);
        assert(va < (unsigned char*)KERNEL_VA_START);
        /*
          store the information about virtual address
         */
        Vir_mem *tmp = kmalloc(sizeof(Vir_mem));
        vir_init(tmp,
            (uint32_t)va, (uint32_t)va + ph_table->memsz, ph_table->flags);
        list_add_after(&vir_range, &(tmp->link));
		/* allocate pages starting from va, with memory size no less than ph->memsz */
        /*
          flags: RWE is the lowest three bits
          TODO: is always user?
         */
        init_msg(m,
            current->pid,
            NEW_PAGE,
            INVALID_ID, (ph_table->flags & 0x2) | (USER_PAGE_ENTRY << 2), pdir, (int)va, ph_table->memsz);
		send(MM, m);
        receive(MM, m);

        pa = m->buf; // pa a is physical address
        // MM should already set the mapping from va to pa,
        // but the pdir is not this process's page directory,
        // using it can't find the right physical address,
        // so have to use physical address directly
        assert(pa >= (unsigned char*)KERNEL_PA_END);
        assert(pa < (unsigned char*)PHY_MEM);
		/* read ph->filesz bytes starting from offset ph->off from file "0" into pa */
        init_msg(m,
            current->pid,
            FM_READ,
            INVALID_ID, name, pa, ph_table->off, ph_table->filesz);
		send(FM, m);
        receive(FM, m);
        // initialize the gap between [file_size, memory_size)
        // all to zero
		for (i = pa + ph_table->filesz; i < pa + ph_table->memsz; *i ++ = 0);
	}

    // initialize the va for stack
    // set the page directory, page table for user stack
    // and allocate page
    /*    va = (unsigned char*)USER_STACK_POINTER;
    assert(va == (unsigned char*)0xbffff000);
    init_msg(m,
        current->pid,
        NEW_PAGE,
        INVALID_ID, INVALID_ID, pdir, (int)va, USER_STACK_SIZE);

    send(MM, m);
    receive(MM, m);
    // TODO which segment, although all segment are
    // the same for time being
    uint32_t ss = SELECTOR_USER(SEG_USER_DATA);
    uint32_t esp = (uint32_t)(va + USER_STACK_SIZE);
    assert(esp == KOFFSET);
    */
    uint32_t ss = 0;
    uint32_t esp = 0;
    create_va_stack(pdir, &ss, &esp);

    // initialize the va for kernel image
    init_kernel_image(pdir);

    // initialize PCB for user process
    void *f = (void*)elf->entry;
    // old way: -- now replaced by create_user_thread
    //PCB* p = create_kthread(f);
    //set_pdir(p, (uint32_t)pdir);
    //set_user_tf(p, ss, esp);
    PCB* p = create_user_thread(f, (uint32_t)pdir, ss, esp, &vir_range);
    add2wake(p);
    // send back
    m->ret = 1;
    m->src = current->pid;
    send(dest, m);
}