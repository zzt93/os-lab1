#include "kernel/kernel.h"
#include "kernel/semaphore.h"
#include "adt/queue.h"
#include "adt/binary_tree.h"

#include "kernel/syscall.h"

Sem wake_lock, sleeped_lock;
/**
   NOTICE:
   In my implementation, when a process is running,
   it will only recorded in the current pointer, not
   in any other queue.
   This process stop running when:
   1. time slot is run out so go to
   schedule(ie, choose another process), I
   will enqueue the running process in wake queue.
   2. sleep, I will add it to sleeped tree

 */

//void vecsys();

int cmp_pid(PCB* a, PCB* b) {
    if (a->pid < b->pid) {
        return -1;
    } else if (a->pid > b->pid) {
        return 1;
    }
    return 0;
}

QUEUE(PCB*, 256, wake)

BI_TREE(PCB*, cmp_pid, sleeped)

PCB idle = {
    .pid = IDLE_ID,
    .state = IDLE,
};
PCB *current = &idle;

/**
   using different choose_process for different schedule algorithm
 */
static PCB* choose_process() {
    if (wake_is_empty()) {
        return &idle;
    }
    PCB* tmp = wake_dequeue();
    return tmp;
}

/*
   implement EDF algorithm to schedule task -- process

   - change wake_dequeue to use priority queue;
   - how to get the period? -- update in task not in here
   - how to find if it is finished
*/
#include "adt/heap.h"

HEAP(PCB*, 256, cmp_pid, process_pri);

PCB* EDF() {
    /*
      for each thread, update priority by deadline -- done by
      task itself
      choose the highest wake thread -- done by priority queue
      may be need a call back method for every process
    */
    int i;
    PCB *p;
    heap_each(i, p) {
        kset_priority(p, -1);
    }
    return process_pri_max();
}

void print_tree(TNode_sleeped* root) {
    if (root == NULL) {
        return;
    }
    assert(root != left(root));
    assert(root != right(root));
    print_tree(left(root));
    printk("t is %d ", root->t->pid);
    print_tree(right(root));
}

/**
   关于页目录, 它是通过CR3寄存器寻找到的.
   由于每个进程都拥有自己的页目录, 所以在进行上下文切换的时候,
   我们必须将被调度进程的页目录基地址载入CR3寄存器, 否则被调度进程将会运行在其它进程的地址空间上, 从而产生错误.
 */
void
schedule(void) {
	/* implement process/thread schedule here */
    NOINTR;
    PROCESS_STATE s = current->state;
    switch(s) {
        case IDLE:
            //        case SLEEPED:
            break;
        case WAKED:
            wake_enqueue(current);
            break;
        case SLEEPED:
            sleeped_add(current);
            printk("add %d to tree\n", current->pid);
            break;
        default:
            assert(false);
    }
    // to balance the lock in do_irq
    current->count_of_lock--;
    //printk("#%d count of lock %d\n", current->pid, current->count_of_lock);
    current = choose_process();
    // load this process's cr3寄存器 when user -> kernel
    // or kernel -> user
    //assert(get_kcr3()->val == current->pdir.val); -- it is
    // only right when current is kernel thread.
    write_cr3(&(current->pdir));
    // set new kernel stack for user process
    /**
       for stack grow down and push will minus first
    then push content in it. --@see push in i386 manual,
    so set esp0 at the end of kernel stack.
    @checked -- setting breakpoint at asm_do_irq() when
    current thread is user test process(13 2015.4),
    by calculating the size of already pushed content
    (including (ss, esp), eflags, cs, eip) and irq_no, error_code
    , we can see the location of trap frame is at esp0 - 4.
    (verify by checking the positions of eflags, cs on the
    kernel stack -- [0xc0000000, 0xc1000000)
    -- using commands `x/10x ...` and `p $eflags`...)
    */
    // set tss esp0 for current thread
    //-- return from user stack to kernel stack
    set_tss_esp0((uint32_t)(current->kstack + KSTACK_SIZE));


    //printk("in queue %d\n", tail-head);
    //printk("in queue %d\n", queue[head]->pid);
    //printk("after add:\n");
    //print_tree(left(sleeped_head));
    printk("Now: current is #%d\n", current->pid);
    NOINTR;
}

void add2wake(PCB* p) {
    lock();
    p->state = WAKED;
    wake_enqueue(p);
    add_process(p);
    unlock();
}
void add2sleeped(PCB* p) {
    lock();
    p->state = SLEEPED;
    sleeped_add(p);
    add_process(p);
    unlock();
}


/*
   Older Version:
   when a thread invoke sleep, I just set the flag
   of process state, and it will be added to sleeped_tree
   at schedule();
 */
/**
   Now:
   after change the state of current state,
   add it to the required queue at once,
   but because I may have already wake up some
   other thread, so it become more inclined to
   cause critical problem.(the other thread may be switched
   when enqueue current thread)
*/
void sleep_to(ListHead* l,
    void (*enqueue)(ListHead*, PCB*)) {
    //print_tree(left(sleeped_head));
    current->state = SLEEPED;
    enqueue(l, current);
    // no need to wait_intr(); for int $0x80
    //wait_intr();
    asm volatile("int $0x80");
    //vecsys(); -- use this is wrong!!!!
    //for no eip, cs, eflags is pushed

}

void sleep() {
    lock();
    //print_tree(left(sleeped_head));
    //printk("#%d in sleep\n", current->pid);
    NOINTR;
    current->state = SLEEPED;
    //    sleeped_add(current);
    //print_tree(left(sleeped_head));
    unlock();
    // no need to wait_intr(); for int $0x80
    //wait_intr();
    // programmed exceptions INT N is unmaskable
    asm volatile("int $0x80": : "a"(SLEEP));
    //vecsys(); -- use this is wrong!!!!
    //for no eip, cs, eflags is pushed
}

void wake_up_from(ListHead* l, PCB* (*dequeue)(ListHead* l)) {
    PCB* wake = dequeue(l);
    if (wake == NULL) {
        printk(RED"Invalid wake up"RESET"\n");
        return;
    }
    wake->state = WAKED;
    wake_enqueue(wake);
}

/**
   to wake_up a process will change the state of this
   process(ie, set the flag) and delete it from sleeped and add
   to wake_queue
 */
/*
void wake_up_lock(PCB* p) {
    lock();
    wake_up_lock(p, 1);
}
*/

void wake_up(PCB* p) {
    lock();
    //delete from sleeped queue
    //printk("#%d in wake\n", current->pid);
    print_tree(left(sleeped_head));
    if (sleeped_delete(p)) {
        p->state = WAKED;
        //printk("wake up %d\n", p->pid);
        //print_tree(left(sleeped_head));
        // add to wake queue
        wake_enqueue(p);
        NOINTR;
    }
    unlock();
}

void delete_ref(PCB *p) {
    lock();
    int res = sleeped_delete(p);
    assert(res != 0);
    res = remove_process(p);
    unlock();
    assert(res != 0);
}