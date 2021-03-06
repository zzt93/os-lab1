#include "c_lib.h"


// return the pid of child process
int fork() {
    return syscall(SYS_fork);
}

int ram_exec(int filename, char *args) {
    return syscall(SYS_ram_exec, filename, args);
}

int exec(char *filename, char *args) {
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

int add_task_ddl(int period) {
    return syscall(SYS_add_task_ddl, period);
}

int put_task_ddl(int period) {
    return syscall(SYS_add_task_ddl, period);
}

int using_edf() {
    return syscall(SYS_using_edf);
}