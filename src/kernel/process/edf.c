#include "kernel/process.h"
#include "adt/heap.h"

static
int cmp_pri(PCB* a, PCB* b) {
	if (a->priority < b->priority) {
        return -1;
    } else if (a->priority > b->priority) {
        return 1;
    }
    return 0;
}

// TODO add to heap at add_process
HEAP(PCB*, 256, cmp_pri, process_pri);

#include "adt/map.h"
// TODO add initial to_ddl at add_process
// pid -> to_ddl
MAP(int, int, to_ddl);

/*
   implement EDF algorithm to schedule task -- process

   - change wake_dequeue to use priority queue;
   - how to get the period? -- update in task not in here
   - how to find if it is finished
*/
PCB* edf() {
    /*
      for each thread, update priority by deadline -- done by
      task itself
      choose the highest wake thread -- done by priority queue
      may be need a call back method for every process
    */
    return process_pri_pop_max();
}

// invoke every second to decrease to_ddl
void to_ddl_each_update() {
    int n = 32;
    int pids[n];
    int res = to_ddl_keySet(pids, n);
    int i, old;
    for (i = 0; i < res; i++) {
        old = to_ddl_get(pids[i]);
        to_ddl_update(pids[i],  old - 1);
    }
}

/**
   set priority and update the position in the heap
 */
void kset_edf_priority(PCB *p, Pri_t priority) {
    assert(priority_in_range(priority));
    p->priority = priority;
    int i;
    PCB *tp;
    heap_each(i, tp) {
        if (tp == p) {
            process_pri_update(i, p);
            return;
        }
    }
}

int process_heap_empty() {
    return process_pri_empty();
}
