#include "kernel/kernel.h"
#include "adt/queue.h"

QUEUE(PCB*, 512)

PCB idle, *current = &idle;

static PCB* choose_process() {
    if (is_empty()) {
        return &idle;
    }
    PCB* tmp = dequeue();
    enqueue(tmp);
    return tmp;
}

void
schedule(void) {
	/* implement process/thread schedule here */
    current = choose_process();
}

void add_process(PCB* p) {
    enqueue(p);
}

