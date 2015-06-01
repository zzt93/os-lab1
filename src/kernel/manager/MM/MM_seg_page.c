#include "kernel/MM.h"
#include "kernel/message.h"
#include "kernel/process.h"

extern PCB* current;

void alloc_page(Msg* m) {
    assert(m->offset >= 0);
    assert(m->len > 0);
    
    // send back
    m->buf = x;
    pid_t dest = m->src;
    m->dest = current->pid;
    send(dest, m);
}