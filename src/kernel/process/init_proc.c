#include "kernel/kernel.h"
#include "lib/malloc.h"
#include "lib/string.h"
#include "adt/list.h"
#include "kernel/semaphore.h"

#include "adt/bit_map.h"
#include "kernel/manager/fd_ft.h"

#define KER_INIT_LOCK 1
#define USER_INIT_LOCK 0

//static int pid_count = START_ID;

BIT_MAP(PID_LIMIT)

int new_id() {
    lock();
    int j = first_val(FREE);
    assert(j != INVALID);
    set_val(j, USED);
    unlock();
    return j + START_ID;
}

void pid_free(int pid) {
    int j = pid - START_ID;
    lock();
    assert(is_val(j, USED));
    set_val(j, FREE);
    unlock();
}


static void init_kernel_tf(TrapFrame* frame, void* fun) {
    // using xxx to represent the second pushed esp
    frame->xxx = (uint32_t)(&(frame->xxx) + 5);// see pushal
    frame->ebp = 0;// @see PM_syscall.c:kfork fork ebp
    assert (frame->xxx == (uint32_t)(&frame->gs));
    frame->cs = SELECTOR_KERNEL(SEG_KERNEL_CODE);
    frame->ds = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->es = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->fs = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->gs = SELECTOR_KERNEL(SEG_KERNEL_DATA);
    frame->eip = (uint32_t)fun;

    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
}

static void init_user_tf(TrapFrame* frame, void* fun) {
    frame->xxx = (uint32_t)(&(frame->xxx) + 5);// see pushal
    frame->ebp = 0;// @see PM_syscall.c:kfork fork ebp
    assert (frame->xxx == (uint32_t)(&frame->gs));
    frame->cs = SELECTOR_USER(SEG_USER_CODE);
    frame->ds = SELECTOR_USER(SEG_USER_DATA);
    frame->es = SELECTOR_USER(SEG_USER_DATA);
    frame->fs = SELECTOR_USER(SEG_USER_DATA);
    frame->gs = SELECTOR_USER(SEG_USER_DATA);
    frame->eip = (uint32_t)fun;

    frame->eflags = 0x200;// set IF = 1, that is enable interrupt
}

void init_fd_table(PCB *pcb, FTE *cwd) {
    // initialize fd table
    lock();
    memset(pcb->fd_table, 0, sizeof(FDE) * PROCESS_MAX_FD);
    assign_fte(&pcb->fd_table[STDIN_FILENO], stdin);
    assign_fte(&pcb->fd_table[STDOUT_FILENO], stdout);
    assign_fte(&pcb->fd_table[STDERR_FILENO], stderr);
    // set the current working directory
    set_cwd(pcb, cwd);
    unlock();
}

static void init_pcb_content(PCB* pcb, uint32_t val, Thread_t type, FTE *cwd) {
    //NOINTR;
    lock();
    pcb->pid = new_id();
    unlock();

    list_init(&(pcb->link));
    list_init(&(pcb->mes));
    //create_sem(&(pcb->mes_lock), 1);
    // default state is no lock, which is different from
    // the first idle process
    pcb->count_of_lock = 0;
    // initialize the page directory address
    set_pdir(pcb, val);
    // kernel thread or user process
    pcb->type = type;
    // process virtual memory range
    list_init(&(pcb->vir_mem));
    // initialize wait pid list
    list_init(&pcb->waitpid);
    init_fd_table(pcb, cwd);

    pcb->priority = (type == KERNEL) ? KERNEL_PRI : USER_PRI;
}


PCB*
create_kthread(void *fun) {
    // malloc PCB
    //NOINTR;
    PCB* pcb = kmalloc(PCB_SIZE);
    //NOINTR;
    // allocate frame at the end of stack, ie frame is [base + all_size - frame_size, base + all_size)
    /**
       @checked after pop all the field in the TrapFrame,
       the esp point to the end of kernel stack
       (set breakpoint in asm_do_irq at iret, the %esp is
       0x14 smaller than the end of kernel stack, which are
       eip, cs, eflags, ss3, esp3)
       (set breakpoint at the start of a thread function, %esp
       is 0x8 smaller than the end of kernel stack, which are
       ss3, esp3)
       notice that in kernel thread, ss3 and esp3 will not be poped
       by hardware so esp will not exactly at the end of kernel stack (kstack)
    */
    TrapFrame *frame = (TrapFrame*)((char *)(pcb->kstack) + KSTACK_SIZE - sizeof(TrapFrame));
    //init trap frame
    init_kernel_tf(frame, fun);
    pcb->tf = frame;

    /**
       have to initialize it again:
       for most kernel process are initialized before
       file_system was loaded, in which default_cwd is
       setted, so for kernel thread, cwd is all invalid
       -- @see file_table.c:init_thread_cwd();
     */
    init_pcb_content(pcb, get_kcr3()->val, KERNEL, default_cwd);
    return pcb;
}

void store_vir(PCB *pcb, ListHead *link) {
    list_add_after(link, &(pcb->vir_mem));
    list_del(link);
}

PCB* create_user_thread(void *f, uint32_t pdir, uint32_t ss, uint32_t esp, ListHead *vir, FTE *cwd) {
    PCB* pcb = kmalloc(PCB_SIZE);
    /**
       trapFrame is always located on the kernel stack
    */
    TrapFrame *frame = (TrapFrame*)((char *)(pcb->kstack) + KSTACK_SIZE - sizeof(TrapFrame)); // allocate frame at the end of stack
    //init trap frame for user
    init_user_tf(frame, f);
    pcb->tf = frame;

    init_pcb_content(pcb, pdir, USER, cwd);
    store_vir(pcb, vir);
    /**
       in order to switch back to user stack
       after interrupt handle
    */
    set_user_stack(pcb, ss, esp);
    return pcb;
}



/**
   NOTICE: when creating a kernel thread which doesn't
   need stack switch, i.e. the real trapFrame to be recovered
   doesn't contain `esp0` and `ss`.
 */
// TODO test
PCB* create_kthread_with_args(void* fun, int arg) {
    PCB* pcb = create_kthread(fun);
    TrapFrame *frame = pcb->tf;

    frame->esp = arg;
    return pcb;
}


void set_esp(PCB *p, uint32_t esp) {
    ((TrapFrame *)p->tf)->esp = esp;
}

