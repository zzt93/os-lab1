#include "kernel/manager/PM_syscall.h"
#include "kernel/manager/manager.h"
#include "kernel/process.h"
#include "lib/string.h"
#include "lib/malloc.h"

static void s_copy(PCB* src, PCB* dest) {
    dest->state = src->state;
    dest->count_of_lock = src->count_of_lock;
    dest->type = src->type;
    assert(src->type == USER);
    dest->pid = new_id();
    assert(dest->pid > src->pid);
}

/**
   TODO check whether every field in pcb is correctly copied
   @see init_proc.c; process.h
 */
// return the pid of child process
void kfork(Msg* m) {
    pid_t fa = m->src;
    PCB *father = m->buf;
    PCB *child = kmalloc(PCB_SIZE);
    // shallow copy: privilege, sign, pid
    s_copy(father, child);
    // deep copy ptable, page when iterate pdir and ptable
    init_meg(m,
        current->pid,
        COPY_page,
        (int)father, (int)child, NULL, INVALID_ID, INVALID_ID);
    send(MM, m);
    receive(MM, m);
    // special handle tf
    TrapFrame *frame = (TrapFrame*)((char *)(child->kstack) + KSTACK_SIZE - sizeof(TrapFrame)); // allocate frame at the end of stack
    // memcpy(frame, father->tf, sizeof(TrapFrame));
    child->tf = frame;

    //reply message
    m->ret = child->pid;
    m->src = current->pid;
    send(fa, m);
}
