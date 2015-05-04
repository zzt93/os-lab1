#include "common.h"
#include "kernel/process.h"

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

void A () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("thread-a\n");
            wake_up(b);
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
            sleep();
        }
        x ++;
    }
}