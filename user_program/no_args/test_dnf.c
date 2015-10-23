#include "sys_call/io/out.h"
#include "kernel/syscall.h"



static
void task(int exe_time, int period) {
    while (1) {
        printf("f execute %d", exe_time);
        wait(period);
    }
}

static
void task_a () {
    task(1, 3);
}

static
void task_b () {
    task(1, 4);
}

static
void task_c () {
    task(2, 5);
}

int entry(char *args) {
    if ((pid = fork()) == 0) {
        task_a();
    } else {
        if ((pid = fork()) == 0) {
            task_b();
        } else {
            task_c();
        }
    }
    return 1;
}