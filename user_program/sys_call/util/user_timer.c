#include "kernel/syscall.h"


int timer_start(int seconds) {
    return syscall(SYS_timer_start, seconds);
}

int timer_finished() {
    return syscall(SYS_timer_finished);
}

int set_timer(int seconds) {
    return syscall(SYS_set_timer, seconds);
}

int wait_timer() {
    return syscall(SYS_wait_timer);
}