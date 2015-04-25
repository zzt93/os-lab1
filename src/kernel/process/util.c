#include "kernel/kernel.h"
#include "test/test_create.h"
#include "lib/malloc.h"

PCB*
create_kthread(void *fun) {
    // malloc PCB
    PCB* pcb = kmalloc(PCB_SIZE);
    TrapFrame *frame = (TrapFrame*)((char *)pcb + PCB_SIZE - 1 - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame
    frame->xxx = (uint32_t)(&(frame->xxx) + 4);// see pushal
    frame->cs = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->ds = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->es = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->fs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->gs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->eip = (uint32_t)fun;
    
    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
    
    pcb->tf = frame;
    return pcb;
}

void
init_proc() {
    create_kthread(A);
    create_kthread(B);
}

