#include "kernel/kernel.h"
#include "test/test_create.h"
#include "lib/malloc.h"

static int pid_count = 0;

PCB*
create_kthread(void *fun) {
    // malloc PCB
    PCB* pcb = kmalloc(PCB_SIZE);
    TrapFrame *frame = (TrapFrame*)((char *)pcb + PCB_SIZE - 1 - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame
    frame->xxx = (uint32_t)(&(frame->xxx) + 4);// see pushal
    assert (frame->xxx == (uint32_t)(&frame->eax));
    frame->cs = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->ds = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->es = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->fs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->gs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->eip = (uint32_t)fun;
    
    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
    
    pcb->tf = frame;
    pcb->pid = pid_count++;
    return pcb;
}

PCB* a;
PCB* b;
PCB* c;
PCB* d;

void
init_proc() {
    b = create_kthread(B);
    add2sleeped(b);
    c = create_kthread(C);
    add2sleeped(c);
    d = create_kthread(D);
    add2sleeped(d);
    a = create_kthread(A);
    add_process(a);
}

/*
void init_proc() {
    add_process(create_kthread(A));
    add_process(create_kthread(B));
}

*/