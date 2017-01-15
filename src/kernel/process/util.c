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
   @Deprecated
   This method is aim to
   create first user process -- shell -- from ram
 */
void ram_user_process() {
    Msg m;
    init_msg(&m,
        current->pid,
        PM_CREATE,
        0, INVALID_ID, NULL, INVALID_ID, INVALID_ID);
    // 0 is the name of shell binary file in ramdisk
    send(PM, &m);
    receive(PM, &m);
    // used when run as a separate thread
    /*
      // should not make it sleeped, for is may make no thread wake
    while (true) {
        wait_intr();
    }
    */
}

#include "kernel/user_process.h"

void user_process() {
    Msg m;
    init_msg(&m,
        current->pid,
        PM_CREATE,
        (int)shell, INVALID_ID, NULL, INVALID_ID, INVALID_ID);
    send(PM, &m);
    receive(PM, &m);
}

void init_driver_test() {
    //add2wake(create_kthread(read_MBR));
    //add2wake(create_kthread(read_FM));
}

/**
   This process is used to prevent the deadlocks
   happened when idle sends message to other thread( so it will
   go to sleep waiting for response)
   to init os and all threads go to sleep
 */
static
void empty() {
    current->state = EDF;
	current->priority = 0;
    while(true) {
        wait_intr();
    }
}

void init_proc_test() {
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

    //add2wake(create_kthread(ram_user_process));

    // to avoid deadlock by an empty process
    add2wake(create_kthread(empty));
}

extern const char *const default_cwd_name;

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
    // this is for test_ch_dir and actually not for kernel thread
    set_cwd_path(current, default_cwd_name);

    add_process(current);
 }

