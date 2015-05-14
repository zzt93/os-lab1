#include "kernel/kernel.h"
#include "test/test_create.h"
#include "lib/malloc.h"
#include "kernel/trapFrame.h"



/*
  // not implement now:
void init_proc() {
    int i;
    for(i = 0; i < 7; i ++) {
        add2wake(create_kthread_with_args(print_ch, 'a' + i));
    }
}
void init_proc() {
    add2wake(create_kthread(keep_stackoverflow));
    add2wake(create_kthread(keep_stackoverflow));
}
*/

PCB* a;
PCB* b;
PCB* c;
PCB* d;


/*
void
init_proc() {
    b = create_kthread(B);
    add2sleeped(b);
    c = create_kthread(C);
    add2sleeped(c);
    d = create_kthread(D);
    add2sleeped(d);
    a = create_kthread(A);
    add2wake(a);
}

void init_proc() {
    add2wake(create_kthread(A));
    add2wake(create_kthread(B));
}

*/

void init_proc() {
    test_setup();
}