#ifndef __PM_H__
#define __PM_H__

#include "common.h"
#include "kernel/message.h"
#include "kernel/process.h"

enum {
    PM_CREATE,
    PM_fork,
    PM_exec,
    PM_ram_exec,
    PM_exit,
    PM_waitpid,
};

void init_PM();

PCB *create_process(Msg *);

void *user_stack_pa(PCB *, uint32_t);

extern int PM;

#include "PM_pdir.h"

#endif /* __PM_H__ */
