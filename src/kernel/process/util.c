#include "kernel/kernel.h"
#include "test/test_create.h"
#include "test/test_drivers.h"
#include "lib/malloc.h"
#include "kernel/init_proc.h"

#include "kernel/process.h"
#include "kernel/message.h"
#include "kernel/manager/PM.h"
#include "kernel/manager/FM.h"

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

/**
   thread to create first user process -- shell
 */
void user_process() {
    Msg m;
    init_msg(&m,
        current->pid,
        PM_CREATE,
        0, INVALID_ID, NULL, INVALID_ID, INVALID_ID);
    send(PM, &m);
    receive(PM, &m);
    while (true) {
        wait_intr();
    }
}

void init_driver_test() {
    //add2wake(create_kthread(read_MBR));
    //add2wake(create_kthread(read_FM));
}

void init_test_proc() {
    //init_driver_test();

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

    add2wake(create_kthread(user_process));
}

void init_idle() {
    // to make it NOINTR
    current->count_of_lock = 1;
    // make it can receive the message so that it can help initialize
    // file system
    list_init(&current->mes);
    // make it can put into waiting queue when waiting receiving
    // message
    current->pdir.val = get_kcr3()->val;
    current->priority = KERNEL_PRI;
}

/*
void init_proc() {
    init_idle();
}
*/