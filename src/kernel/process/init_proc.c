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
    frame->cs = SELECTOR_KERNEL(SEG_KERNEL_CODE);
    frame->ds = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->es = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->fs = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->gs = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->eip = (uint32_t)fun;

    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
}

void init_user_tf(TrapFrame* frame, void* fun) {
    frame->xxx = (uint32_t)(&(frame->xxx) + 5);// see pushal
    assert (frame->xxx == (uint32_t)(&frame->gs));
    frame->cs = SELECTOR_USER(SEG_USER_CODE);
    frame->ds = SELECTOR_USER(SEG_USER_DATA);
    frame->es = SELECTOR_USER(SEG_USER_DATA);
    frame->fs = SELECTOR_USER(SEG_USER_DATA);
    frame->gs = SELECTOR_USER(SEG_USER_DATA);
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
    assert((val&0xfff) == 0);
    p->pdir.val = 0;
    p->pdir.page_directory_base = val >> 12;
}

static void init_pcb_content(PCB* pcb, uint32_t val, Thread_t type) {
    //NOINTR;
    lock();
    pcb->pid = pid_count++;
    unlock();

    list_init(&(pcb->link));
    list_init(&(pcb->mes));
    //create_sem(&(pcb->mes_lock), 1);
    pcb->count_of_lock = 0;
    // initialize the page directory address
    set_pdir(pcb, val);
    // kernel thread or user process
    pcb->type = type;
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

    init_pcb_content(pcb, get_kcr3()->val, KERNEL);
    return pcb;
}


PCB* create_user_thread(void *f, uint32_t pdir, uint32_t ss, uint32_t esp) {
    PCB* pcb = kmalloc(PCB_SIZE);
    /**
       trapFrame is always located on the kernel stack
     */
    TrapFrame *frame = (TrapFrame*)((char *)(pcb->kstack) + KSTACK_SIZE - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame for user
    init_user_tf(frame, f);
    pcb->tf = frame;

    init_pcb_content(pcb, pdir, USER);
    /**
       in order to switch back to user stack
     */
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

    init_pcb_content(pcb, get_kcr3()->val, KERNEL);
    return pcb;
}