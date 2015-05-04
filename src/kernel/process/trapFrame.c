#include "kernel/kernel.h"
#include "lib/malloc.h"

static int pid_count = 0;

static void init_kernel_tf(TrapFrame* frame, void* fun) {
    frame->xxx = (uint32_t)(&(frame->xxx) + 4);// see pushal
    assert (frame->xxx == (uint32_t)(&frame->eax));
    frame->cs = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->ds = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->es = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->fs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->gs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->eip = (uint32_t)fun;

    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
}


PCB*
create_kthread(void *fun) {
    // malloc PCB
    PCB* pcb = kmalloc(PCB_SIZE);
    TrapFrame *frame = (TrapFrame*)((char *)pcb + PCB_SIZE - 1 - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame
    init_kernel_tf(frame, fun);
    pcb->tf = frame;
    pcb->pid = pid_count++;
    return pcb;
}

PCB* create_kthread_with_args(void* fun, int arg) {
    PCB* pcb = kmalloc(PCB_SIZE);
    void *last = (char*)pcb + PCB_SIZE - 1;
    *((int *)last) = arg;
    TrapFrame *frame = (TrapFrame*)(last - sizeof(TrapFrame) - sizeof(arg));
    init_kernel_tf(frame, fun);
    pcb->tf = frame;
    pcb->pid = pid_count++;
    return pcb;
}