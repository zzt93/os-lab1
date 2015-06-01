#include "kernel/process.h"
#include "kernel/init_proc.h"
#include "kernel/message.h"

#include "kernel/manager.h"

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
        assert(va >= (unsigned char*)0);
        assert(va <= (unsigned char*)0xc0000000);
		/* allocate pages starting from va, with memory size no less than ph->memsz */
        init_meg(m,
            NEW_PAGE,
            INVALID_ID, INVALID_ID, NULL, (int)va, ph_table->memsz);
		send(MM, m);
        receive(MM, m);

        pa = m->buf;
        assert(pa >= (unsigned char*)0);
        assert(pa <= (unsigned char*)0xc0000000);
		/* read ph->filesz bytes starting from offset ph->off from file "0" into pa */
        init_meg(m,
            FM_READ,
            INVALID_ID, INVALID_ID, pa, ph_table->off, ph_table->filesz);
		send(FM, m);
        receive(FM, m);
        // initialize the gap between [file_size, memory_size]
        // all to zero
		for (i = pa + ph_table->filesz; i < pa + ph_table->memsz; *i ++ = 0);
	}

    // send back
    void *f = (void*)elf->entry;
    add2wake(create_kthread(f));
    m->ret = 1;
    int dest = m->src;
    m->src = current->pid;
    send(dest, m);
}