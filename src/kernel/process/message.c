#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/semaphore.h"
#include "lib/string.h"

//static Sem empty, full;

void add_message(PCB* p, Msg* m) {
    list_add_after(&(p->mes), &(m->list));
}

int no_message(PCB* p) {
    return list_empty(&(p->mes));
}

void get_message(PCB* p, pid_t id, Msg* m) {
    // find and delete aim message
    ListHead* head = &(p->mes);
    ListHead* ptr = NULL;
    list_foreach(ptr, head) {
        
        if (ptr ) {
            break;
        }
    }
    Msg* aim = NULL;
    // copy to m
    memcpy(m, aim, sizeof(Sem));
}

void send(pid_t dest, Msg *m) {
    /**
       TODO add semaphore or lock
     */
    PCB* de = fetch_pcb(dest);
    Sem* s = &(de->mes_lock);
    P(s);
    add_message(de, m);
    V(s);
    //V(&full);
}

void receive(pid_t src, Msg *m) {
    Sem* s = &(current->mes_lock);
    //P(&full);
    P(s);
    get_message(current, src, m);
    V(s);
    //V(&empty);
}