#include "kernel/kernel.h"
#include "test/test_create.h"

PCB*
create_kthread(void *fun) {
    //init trap frame
    // esp = fun
    // tf = tail of trap frame
    return NULL;
}

void
init_proc() {
    create_kthread(A);
    create_kthread(B);
}

