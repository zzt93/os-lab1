#include "kernel/kernel.h"
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
   2. sleep, I will add it to blocked tree
   
 */

void vecsys();

int cmp_pid(PCB* a, PCB* b) {
    if (a->pid < b->pid) {
        return -1;
    } else if (a->pid > b->pid) {
        return 1;
    }
    return 0;
}

QUEUE(PCB*, 256, wake)

BI_TREE(PCB*, cmp_pid, blocked)

PCB idle = {.pid = -1,
            .state = IDLE,
};
PCB *current = &idle;

static PCB* choose_process() {
    if (wake_is_empty()) {
        return &idle;
    }
    PCB* tmp = wake_dequeue();
    //wake_enqueue(tmp);
    return tmp;
}

void
schedule(void) {
	/* implement process/thread schedule here */
    printk("schedule:\n current is #%d\n", current->pid);
    PROCESS_STATE s = current->state;
    switch(s) {
        case IDLE:
            break;
        case WAKED:
            wake_enqueue(current);
            break;
        case SLEEPED:
            blocked_add(current);
            break;
        default:
            assert(false);
    }
    printk("size of queue %d\n", wake_size());
    current = choose_process();
}

void add_process(PCB* p) {
    p->state = WAKED;
    wake_enqueue(p);
}
void add2blocked(PCB* p) {
    p->state = SLEEPED;
    blocked_add(p);
}

static void print_tree(TNode_blocked* root) {
    if (root == NULL) {
        return;
    }
    print_tree(left(root));
    printk("t is %d\n", root->t->pid);
    print_tree(right(root));
}

/**
   when a thread invoke sleep, I just set the flag
   of process state, and it will be added to blocked_tree
   at schedule();
 */
void sleep() {
    print_tree(left(blocked_head));
    current->state = SLEEPED;
    //blocked_add(current);
    // delete from wake queue
    //PCB* t = wake_pop_last();
    //assert(t == current);
    wait_intr();
    vecsys();
}

/**
   to wake_up a process will change the state of this
   process(ie, set the flag) and delete it from blocked and add
   to wake_queue
 */
void wake_up(PCB* p) {
    //delete from blocked queue
    print_tree(left(blocked_head));
    blocked_delete(p);
    p->state = WAKED;
    print_tree(left(blocked_head));
    // add to wake queue
    wake_enqueue(p);
}