#include "kernel/syscall.h"


// return the pid of child process
int fork() {
    return syscall(SYS_fork);
}