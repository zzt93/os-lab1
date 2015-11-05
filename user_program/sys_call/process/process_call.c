#include "kernel/syscall.h"


// return the pid of child process
int fork() {
    return syscall(SYS_fork);
}

int exec(int filename, char *args) {
    return syscall(SYS_exec, filename, args);
}

int exit() {
    return syscall(SYS_exit);
}

int getpid() {
    return syscall(SYS_getpid);
}

int waitpid(int wait) {
    return syscall(SYS_waitpid, wait);
}

int wait(int time) {
    return syscall(SYS_wait, time);
}

int set_priority(Pri_t priority) {
    return syscall(SYS_set_priority, priority);
}

int get_priority() {
    return syscall(SYS_get_priority);
}

