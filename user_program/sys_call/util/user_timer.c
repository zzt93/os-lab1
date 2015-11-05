#include "kernel/syscall.h"


int timer_start(int seconds) {
    return syscall(SYS_timer_start, seconds);
}

int timer_finished() {
    return syscall(SYS_timer_finished);
}