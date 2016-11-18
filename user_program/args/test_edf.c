#include "kernel/syscall.h"

#include "lib/string.h"
#include "lib/math.h"


#include "sys_call/io/out.h"

#define ARGS_MAX_LEN 128

int init_p;

static
void task(const char *name, int exe_time, int period) {
    //put_task_ddl(period);
    while (1) {
        // this timer count the overall time
        set_timer(period);
        // this timer count the timer this process using
        timer_start(exe_time);
        while (!timer_finished()) {
        }
        printf("%s execute #%d second(s)\n", name, exe_time);
        add_task_ddl(period);
        wait_timer();
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
    int len = strlen(args);
    int count;
    char *save[NUM_TASKS * 2] = {0};
    char copy[ARGS_MAX_LEN];
    memcpy(copy, args, len);
    count = split(copy, ' ', save);
    if (count != NUM_TASKS * 2) {
        printf("wrong number of arguments, should be %d arguments\n", NUM_TASKS * 2);
        return 0;
    }
    int i;
    int i_arg[count];
    for (i = 0; i < count; i++) {
        i_arg[i] = to_int(save[i]);
    }
    if(!can_edf(i_arg)) {
        printf("wrong arguments: can't make a edf schedule\n");
        return 0;
    }

    // make it move from wake_queue to process_pri_heap
    using_edf();

    //init_p = get_priority();
    int pid;
    if ((pid = fork()) == 0) {
        // set to_ddl will update priority automatically
        //set_priority(init_p - i_arg[1]);
        put_task_ddl(i_arg[1]);
        task("a", i_arg[0], i_arg[1]);
    } else {
        if ((pid = fork()) == 0) {
            //set_priority(init_p - i_arg[3]);
            put_task_ddl(i_arg[3]);
            task("b", i_arg[2], i_arg[3]);
        } else {
            //set_priority(init_p - i_arg[5]);
            put_task_ddl(i_arg[5]);
            task("c", i_arg[4], i_arg[5]);
        }
    }
    return 1;
}