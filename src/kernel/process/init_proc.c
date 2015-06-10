#include "kernel/kernel.h"
#include "lib/malloc.h"
#include "adt/list.h"
#include "kernel/semaphore.h"

#define KER_INIT_LOCK 1
#define USER_INIT_LOCK 0

static int pid_count = START_ID;

static void init_kernel_tf(TrapFrame* frame, void* fun) {
    frame->xxx = (uint32_t)(&(frame->xxx) + 5);// see pushal
    assert (frame->xxx == (uint32_t)(&frame->gs));
    frame->cs = (SEG_KERNEL_CODE << 3) + (0 << 2) + 0;
    frame->ds = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->es = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->fs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->gs = (SEG_KERNEL_DATA << 3) + (0 << 2) + 0;
    frame->eip = (uint32_t)fun;

    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
}

/**
   set the tf store the content of user stack
   for user process running on it
 */
static inline void set_user_stack(PCB* p, uint32_t ss, uint32_t esp) {
    TrapFrame* frame = (TrapFrame*)p->tf;
    frame->ss = ss;
    frame->esp = esp;
}

static inline void set_pdir(PCB* p, uint32_t val) {
    p->pdir.val = 0;
    p->pdir.page_directory_base = val >> 12;
}

static void init_pcb_content(PCB* pcb, uint32_t val) {
    //NOINTR;
    lock();
    pcb->pid = pid_count++;
    unlock();

    list_init(&(pcb->link));
    list_init(&(pcb->mes));
    //create_sem(&(pcb->mes_lock), 1);
    pcb->count_of_lock = 0;
    // initialize the page directory address
    assert((val&0xfff) == 0);
    //pcb->pdir.val = val;
    set_pdir(pcb, val);
}

PCB*
create_kthread(void *fun) {
    // malloc PCB
    //NOINTR;
    PCB* pcb = kmalloc(PCB_SIZE);
    //NOINTR;
    TrapFrame *frame = (TrapFrame*)((char *)(pcb->kstack) + KSTACK_SIZE - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame
    init_kernel_tf(frame, fun);
    pcb->tf = frame;

    init_pcb_content(pcb, get_kcr3()->val);
    return pcb;
}


PCB* create_user_thread(void *f, uint32_t pdir, uint32_t ss, uint32_t esp) {
    PCB* pcb = kmalloc(PCB_SIZE);
    TrapFrame *frame = (TrapFrame*)((char *)(pcb->kstack) + KSTACK_SIZE - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame
    init_kernel_tf(frame, f);
    pcb->tf = frame;

    init_pcb_content(pcb, pdir);
    set_user_stack(pcb, ss, esp);
    return pcb;
}


PCB* create_kthread_with_args(void* fun, int arg) {
    PCB* pcb = kmalloc(PCB_SIZE);
    // the address which out of boundary
    void *last = (char*)(pcb->kstack) + KSTACK_SIZE;
    // move back 32bits to fit a int
    *((int *)last - 1) = arg;
    TrapFrame *frame = (TrapFrame*)(last - sizeof(TrapFrame) - sizeof(arg));
    init_kernel_tf(frame, fun);
    pcb->tf = frame;

    init_pcb_content(pcb, get_kcr3()->val);
    return pcb;
}