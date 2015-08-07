#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "x86/memory.h"
#include "assert.h"

int syscall(int, ...);
void do_syscall(TrapFrame*);

enum {
    SYS_read,
    SYS_write,
    // FM and PM delimiter
    FM_PM,
    // process related system call
    SYS_exit,
    SYS_fork,
    SYS_exec,
    SYS_getpid,
    SYS_waitpid,
    // Miscellaneous
    MIS,
    // user test
    SYS_print_serial,
    SYS_printf,
    SYS_read_line,
    SYS_wait,
    // for int 0x80 in sleep method
    // @see ./src/kernel/process/schedule.c
    SLEEP,
};

int fork();
int exec(int filename, char *args);
int exit();
int getpid();
int waitpid(int wait);
int wait(int time);


#endif /* __SYSCALL_H__ */