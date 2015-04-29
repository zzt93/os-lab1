#include "kernel/kernel.h"
#include "adt/queue.h"

QUEUE(PCB*, 512, wake)

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

