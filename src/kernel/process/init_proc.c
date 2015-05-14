#include "kernel/kernel.h"
#include "lib/malloc.h"
#include "adt/list.h"
#include "kernel/semaphore.h"

static int pid_count = START_ID;

static void init_kernel_tf(TrapFrame* frame, void* fun) {
    frame->xxx = (uint32_t)(&(frame->xxx) + 5);// see pushal
    assert (frame->xxx == (uint32_t)(&frame->gs));
    frame->cs = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->ds = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->es = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->fs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->gs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->eip = (uint32_t)fun;

    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
}

static void init_pcb_content(PCB* pcb) {
    pcb->pid = pid_count++;

    list_init(&(pcb->link));
    list_init(&(pcb->mes));
    create_sem(&(pcb->mes_lock), 1);
}

PCB*
create_kthread(void *fun) {
    // malloc PCB
    PCB* pcb = kmalloc(PCB_SIZE);
    TrapFrame *frame = (TrapFrame*)((char *)(pcb->kstack) + KSTACK_SIZE - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame
    init_kernel_tf(frame, fun);
    pcb->tf = frame;

    init_pcb_content(pcb);
    return pcb;
}

PCB* create_kthread_with_args(void* fun, int arg) {
    PCB* pcb = kmalloc(PCB_SIZE);
    void *last = (char*)(pcb->kstack) + KSTACK_SIZE;
    *((int *)last - 1) = arg;
    TrapFrame *frame = (TrapFrame*)(last - sizeof(TrapFrame) - sizeof(arg));
    init_kernel_tf(frame, fun);
    pcb->tf = frame;

    init_pcb_content(pcb);
    return pcb;
}