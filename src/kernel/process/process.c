#include "kernel/process.h"
#include "adt/map.h"

MAP(int, PCB*, all)


void add_process(PCB *pcb) {
    assert(pcb != NULL);
    all_put(pcb->pid, pcb);
}

PCB *fetch_pcb(pid_t id) {
    //printk("fetch id %d", id);
    return all_get(id);
}

/**
   write as more as possible pcb's pointer into `pcbs`.
     @return how many pcbs are fetched
 */
int fetch_all_pcb(PCB **pcbs, int capacity) {
    return all_values(pcbs, capacity);
}

size_t pcb_size() {
    int size = all_get_map_size();
    assert(size >= 0);
    return size;
}

int remove_process(PCB *pcb) {
    assert(pcb != NULL);
    return all_remove(pcb->pid);
}

/**
   TODO deadlock detection: add a field in PCB for waiting process

   // pid_t waiting_for;
   when to update this field:
   - adding to sleeped tree
   -- send msg to some process: waiting_for = pid;
   -- receive the msg: waiting_for = INVALID_PID
   -- timer kwait() (covered by sending and receiving)
   - block on process->link
   -- P() & V()
   - block on process->waitpid
   -- waitpid()

   check whether this make a cycle. if it make, choose a victim
   and exit it.
 */