#include "kernel/syscall.h"


// return the pid of child process
int fork() {
    return syscall(SYS_fork);
}

int exec(int filename, char *args) {
    return syscall(SYS_exec, filename, args);
}