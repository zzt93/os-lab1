#include "kernel/kernel.h"
#include "kernel/semaphore.h"
#include "adt/queue.h"
#include "adt/binary_tree.h"


/**
   NOTICE:
   In my implementation, when a process is running,
   it will only recorded in the current pointer, not
   in any other queue.
   Only when this process stop running:
   1. schedule(ie, choose another process), I
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

PCB idle = {.pid = IDLE_ID,
            .state = IDLE,
};
PCB *current = &idle;

static PCB* choose_process() {
    if (wake_is_empty()) {
        return &idle;
    }
    PCB* tmp = wake_dequeue();
    return tmp;
}


static void print_tree(TNode_sleeped* root) {
    if (root == NULL) {
        return;
    }
    print_tree(left(root));
    printk("t is %d\n", root->t->pid);
    print_tree(right(root));
}


void
schedule(void) {
	/* implement process/thread schedule here */
    //printk("Before schedule: current is #%d\n", current->pid);
    PROCESS_STATE s = current->state;
    switch(s) {
        case IDLE:
        case SLEEPED:
            break;
        case WAKED:
            wake_enqueue(current);
            break;
            /*
        case SLEEPED:
            sleeped_add(current);
            break;
            */
        default:
            assert(false);
    }
    //printk("in queue %d\n", queue[head]->pid);
    current = choose_process();
    //printk("in tree:\n");
    //print_tree(left(sleeped_head));
    //printk("Now: current is #%d\n", current->pid);
}

void add_process(PCB* p) {
    p->state = WAKED;
    wake_enqueue(p);
}
void add2sleeped(PCB* p) {
    p->state = SLEEPED;
    sleeped_add(p);
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
    print_tree(left(sleeped_head));
    current->state = SLEEPED;
    sleeped_add(current);
    print_tree(left(sleeped_head));
    unlock();
    // no need to wait_intr(); for int $0x80
    //wait_intr();
    asm volatile("int $0x80");
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
void wake_up(PCB* p) {
    lock();
    //delete from sleeped queue
    print_tree(left(sleeped_head));
    if (sleeped_delete(p)) {
        p->state = WAKED;
        print_tree(left(sleeped_head));
        // add to wake queue
        wake_enqueue(p);
    }
    unlock();
}