#include "sys_call/io/out.h"
#include "kernel/syscall.h"

#include "lib/string.h"


static
void task(int exe_time, int period) {
    while (1) {
        printf("f execute %d", exe_time);
        wait(period);
    }
}

#define NUM_TASKS 3

static
int can_edf(int *arg) {
    int lcm = LCM(arg[1], arg[3], arg[5]);
    int i, j;
    int sum = 0;
    for (i = 0; i < NUM_TASKS; i++) {
        j = i * 2;
        sum += lcm / arg[1 + j] * arg[j];
    }
    if (sum > lcm) {
        return false;
    }
    return true;
}

/**
   Usage:
   2 t1-e-time t1-period t2-e-time t2-period t3-e-time t3-period
 */
int entry(char *args) {
    int count;
    char *save[NUM_TASKS * 2] = {0};
    memcpy(copy, args, BUF_SZ);
    count = split(copy, ' ', save);
    assert(count == NUM_TASKS * 2);
    int i;
    int arg[count];
    for (i = 0; i < count; i++) {
        arg[i] = to_int(save[i]);
    }
    if(!can_edf(arg)) {
        return 0;
    }

    if ((pid = fork()) == 0) {
        task_a(arg[0], arg[1]);
    } else {
        if ((pid = fork()) == 0) {
            task_b(arg[2], arg[3]);
        } else {
            task_c(arg[4], arg[5]);
        }
    }
    return 1;
}