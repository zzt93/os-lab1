#include "kernel/process.h"
#include "kernel/init_proc.h"
#include "kernel/message.h"

#include "kernel/manager.h"

#include "kernel/memory.h"

#define B_SIZE 512

extern PCB* current;

int PM;

/**
   The message sent to PM should specify:
   m->type
   m->i[0] -- the program/file name

   The message send from PM specify:
   m->ret -- whether successfully create a user process
*/
static void PM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        switch(m.type) {
            case PM_CREATE:
                create_process(&m);
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


/**
   whether to use this message to send
 */
void create_process(Msg* m) {
    int name = m->i[0];
    assert(name >= 0);
    char buf[B_SIZE];

    // create page directory for new process
    uint32_t pdir = ;
    assert(pdir&0xfff == 0);
    /* read 512 bytes starting from offset 0 from file "0" into buf */
	/* it contains the ELF header and program header table */
    init_meg(m,
        FM_READ,
        INVALID_ID, INVALID_ID, buf, 0 + name, B_SIZE);
	send(FM, m);
    receive(FM, m);

    struct ELFHeader *elf = (struct ELFHeader*)buf;
    struct ProgramHeader *ph_table, *end_ph;
    unsigned char *va, *pa, *i;

	ph_table = (struct ProgramHeader*)((char *)elf + elf->phoff);
    end_ph = ph_table + elf->phnum;
	for (; ph_table < end_ph; ph_table++) {
		/* scan the program header table, load each segment into memory */

		va = (unsigned char*)ph_table->vaddr; /* virtual address */
        //assert(va >= 0x08048000);
        assert(va > (unsigned char*)0);
        assert(va < (unsigned char*)KERNEL_VA_START);
		/* allocate pages starting from va, with memory size no less than ph->memsz */
        init_meg(m,
            NEW_PAGE,
            INVALID_ID, INVALID_ID, pdir, (int)va, ph_table->memsz);
		send(MM, m);
        receive(MM, m);

        pa = m->buf; // TODO pa is physical address???
        // MM should already set the mapping from va to pa,
        // but the pdir is not this process's page directory,
        // using it can't find the right physical address,
        // so have to use physical address directly
        assert(pa > (unsigned char*)KERNEL_PA_END);
        assert(pa < (unsigned char*)PHY_MEM);
		/* read ph->filesz bytes starting from offset ph->off from file "0" into pa */
        init_meg(m,
            FM_READ,
            INVALID_ID, INVALID_ID, pa, ph_table->off, ph_table->filesz);
		send(FM, m);
        receive(FM, m);
        // initialize the gap between [file_size, memory_size)
        // all to zero
		for (i = pa + ph_table->filesz; i < pa + ph_table->memsz; *i ++ = 0);
	}

    // initialize PCB for user process
    void *f = (void*)elf->entry;
    // TODO using create_kthread???
    PCB* p = create_kthread(f);
    set_pdir(p, pdir);
    add2wake(p);
    // send back
    m->ret = 1;
    int dest = m->src;
    m->src = current->pid;
    send(dest, m);
}