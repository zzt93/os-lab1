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
       @return: Returns success unless COMMAND is not found or a redirection error occurs.
       m->ret -- the status of execution
*/
static void PM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        pid_t dest = m.src;
        switch(m.type) {
            case PM_CREATE:
            {
                PCB *p = create_process(&m);
                if (p == NULL) {
                    m.ret = FAIL;
                } else {
                    add2wake(p);
                    m.ret = SUCC;
                }
                break;
            }
            case PM_fork:
            {
                PCB * new = kfork(&m);
                assert(new != NULL);
                add2wake(new);
                pid_t child = new->pid;
                // reply to father
                m.ret = child;
                break;
            }
            case PM_exec:
            {
                PCB *new = kexec(&m);
                if (new != NULL) {
                    // put in queue
                    add2wake(new);
                }
                // for the thread apply exec is now not exist,
                // so no need to send back
                continue;
            }
            case PM_exit:
                kexit(&m);
                // for the thread apply exec is now not exist,
                // so no need to send back
                continue;
            case PM_waitpid:
                kwaitpid(&m);
                continue;
            default:
                assert(false);
                break;
        }
        m.src = current->pid;
        send(dest, &m);
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
   user stack initial page end position -- must minus some number
   for it is out of range
   user stack initial page starting position
 */
void * user_stack_pa(PCB *p, uint32_t val) {
    return get_pa(&p->pdir, val);
}
/**
   single responsibility:
   just create process,
   not add it to wake queue
   not send message back
 */
PCB *create_process(Msg* m) {
    int name = m->i[0];
    assert(name >= 0);
    char buf[B_SIZE];

    // create page directory for new process
    // must using physical address for cr3 should only store pa
    PDE* pdir = pdir_alloc();
    assert( ((int)pdir&0xfff) == 0);
    /* read 512 bytes starting from offset 0 from file "name" into buf */
	/* it contains the ELF header and program header table */
    init_msg(m,
        current->pid,
        FM_read,
        INVALID_ID, name, buf, 0, B_SIZE);
	send(FM, m);
    receive(FM, m);
    if (m->ret == FAIL) {
        return NULL;
    }

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
        Seg_info *tmp = kmalloc(sizeof(Seg_info));
        vir_init(tmp,
            (uint32_t)va, (uint32_t)va + ph_table->memsz, ph_table->flags);
        list_add_before(&vir_range, &(tmp->link));
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
            FM_read,
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
    PCB* p = create_user_thread(f, (uint32_t)pdir, ss, esp, &vir_range, default_cwd);
    return p;
}