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
    SYS_sleep,
    // Miscellaneous
    MIS,
    // user test
    SYS_print_serial,
    SLEEP,
};



#endif /* __SYSCALL_H__ */