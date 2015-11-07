#include "kernel/process.h"
#include "adt/heap.h"

int cmp_pid(PCB* a, PCB* b);

// TODO add to heap at add_process
HEAP(PCB*, 256, cmp_pid, process_pri);

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
PCB* EDF() {
    /*
      for each thread, update priority by deadline -- done by
      task itself
      choose the highest wake thread -- done by priority queue
      may be need a call back method for every process
    */
    int i;
    PCB *p;
    int to_ddl;
    heap_each(i, p) {
        to_ddl = to_ddl_get(p->pid);
        assert(priority_in_range(USER_PRI - to_ddl));
        p->priority = USER_PRI - to_ddl;
        process_pri_update(i, p);
    }
    return process_pri_max();
}

// invoke every second to decrease to_ddl
void to_ddl_each_update() {
    int n = 32;
    int pids[n];
    int res = to_ddl_keySet(pids, n);
    int i;
    for (i = 0; i < res; i++) {
        to_ddl_update(pids[i], to_ddl_get(pids[i]) - 1);
    }
}

/**
   set priority and update the position in the heap
 */
void kset_priority(PCB *p, Pri_t priority) {
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
