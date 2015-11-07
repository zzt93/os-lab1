#include "kernel/syscall.h"

#include "lib/string.h"
#include "lib/math.h"


#include "sys_call/io/out.h"

#define BUF_SZ 128

int init_p;

static
void task(const char *name, int exe_time, int period) {
    while (1) {
        timer_start(exe_time);
        while (!timer_finished()) {
        }
        printf("%s execute #%d second(s)\n", name, exe_time);
        update_task_ddl(period);
        wait(period);
    }
}

#define NUM_TASKS 3

static
int can_edf(int *arg) {
    int lcm = LCM( LCM(arg[1], arg[3]), arg[5]);
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
   example:
   2 1 3 2 4 3 5
 */
int entry(char *args) {
    int count;
    char *save[NUM_TASKS * 2] = {0};
    char copy[BUF_SZ];
    memcpy(copy, args, BUF_SZ);
    count = split(copy, ' ', save);
    user_assert(count == NUM_TASKS * 2);
    int i;
    int i_arg[count];
    for (i = 0; i < count; i++) {
        i_arg[i] = to_int(save[i]);
    }
    if(!can_edf(i_arg)) {
        return 0;
    }

    init_p = get_priority();
    int pid;
    if ((pid = fork()) == 0) {
        set_priority(init_p - i_arg[1]);
        task("a", i_arg[0], i_arg[1]);
    } else {
        if ((pid = fork()) == 0) {
            set_priority(init_p - i_arg[3]);
            task("b", i_arg[2], i_arg[3]);
        } else {
            set_priority(init_p - i_arg[5]);
            task("c", i_arg[4], i_arg[5]);
        }
    }
    return 1;
}