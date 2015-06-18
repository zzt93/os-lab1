#include "common.h"
#include "kernel/process.h"
#include "kernel/message.h"
#include "kernel/manager/PM.h"

/*
void A () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {printk("a");}
        x ++;
    }
}

void B () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {printk("b");}
        x ++;
    }
}
*/

extern PCB* a;
extern PCB* b;
extern PCB* c;
extern PCB* d;

/**
   if this thread is interrupted exactly after
   it wake_up next thread and before A going to
   sleep, thread-a will put into waked queue rather than
   sleeped tree, which might cause B can't find A in sleeped
   which cause thread running not in order -- but still can run
*/
/*
void A () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("thread-a\n");
            wake_up(b);
            INTR;
            //asm volatile("int $0x80");
            sleep();
        }
        x ++;
    }
}
void B () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("thread-b\n");
            wake_up(c);
            INTR;
            //asm volatile("int $0x80");
            sleep();
        }
        x ++;
    }
}
void C () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("thread-c\n");
            wake_up(d);
            INTR;
            //asm volatile("int $0x80");
            sleep();
        }
        x ++;
    }
}
void D () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("thread-d\n");
            wake_up(a);
            INTR;
            //asm volatile("int $0x80");
            sleep();
        }
        x ++;
    }
}
*/


void user_process() {
    Msg m;
    init_meg(&m,
        current->pid,
        PM_CREATE,
        0, INVALID_ID, NULL, INVALID_ID, INVALID_ID);
    send(PM, &m);
    while (true) {
        wait_intr();
    }
}