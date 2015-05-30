#include "kernel/kernel.h"
#include "test/test_create.h"
#include "test/test_drivers.h"
#include "lib/malloc.h"
#include "kernel/init_proc.h"


/*
  // not correctly implemented now:
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
*/

/*
void init_proc() {
    add2wake(create_kthread(A));
    add2wake(create_kthread(B));
}

void init_proc() {
    test_setup();
}

*/
void init_idle() {
    current->count_of_lock = 1;
}

void init_driver_test() {
    //add2wake(create_kthread(read_MBR));
    add2wake(create_kthread(read_FM));
}

void init_test_proc() {
    init_driver_test();
}

void init_proc() {
    init_idle();

    /* test for lock and semaphore
    create_sem(&wake_lock, 1);
    create_sem(&sleeped_lock, 1);

    add2wake(create_kthread(A));
    NOINTR;
    add2wake(create_kthread(B));
    add2wake(create_kthread(C));
    add2wake(create_kthread(D));
    add2wake(create_kthread(E));
    */
}
/*
*/