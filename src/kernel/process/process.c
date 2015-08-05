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

int remove_process(PCB *pcb) {
    assert(pcb != NULL);
    return all_remove(pcb->pid);
}