#include "kernel/kernel.h"
#include "adt/queue.h"

void vecsys();

QUEUE(PCB*, 256, wake)
//QUEUE(PCB*, 256, blocked)

PCB idle, *current = &idle;

static PCB* choose_process() {
    if (wake_is_empty()) {
        return &idle;
    }
    PCB* tmp = wake_dequeue();
    wake_enqueue(tmp);
    return tmp;
}

void
schedule(void) {
	/* implement process/thread schedule here */
    current = choose_process();
}

void add_process(PCB* p) {
    wake_enqueue(p);
}


void sleep() {
    //blocked_enqueue(current);
    // delete from wake queue
    
    wait_intr();
    vecsys();
}

void wake_up(PCB* p) {
    //delete from blocked queue
    //blocked_delete(p);
    // add to wake queue
    wake_enqueue(p);
}