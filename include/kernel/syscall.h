#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "x86/memory.h"

void int syscall(int, ...);
void do_syscall(TrapFrame*);

enum {
    SYS_read,
    SYS_write,
    SYS_exit,
    SYS_fork,
    SYS_exec,
    SYS_sleep,
};

#endif /* __SYSCALL_H__ */