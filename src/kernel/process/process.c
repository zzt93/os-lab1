#include "kernel/process.h"
#include "adt/map.h"

MAP(int, PCB*, all)


void add_process(PCB* pcb) {
    assert(pcb != NULL);
    all_put(pcb->pid, pcb);
}

PCB* fetch_pcb(pid_t id) {
    //printk("fetch id %d", id);
    return all_get(id);
}

/**
   write as more as possible pcb's pointer into `pcbs`.
   if the array of pcb is not enough to contain all,
   it will return the number of pcbs which are left.
   i.e. return capacity - map_contain
 */
int fetch_all_pcb(PCB **pcbs, int capacity) {
    int c = capacity;
    all_values(pcbs, &c);
    return c;
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