#ifndef __PM_H__
#define __PM_H__

#include "common.h"
#include "kernel/message.h"
#include "kernel/process.h"

enum {
    PM_CREATE,
    PM_fork,
    PM_exec,
    PM_exit,
};

void init_PM();
PCB *create_process(Msg*);

extern int PM;

#include "PM_pdir.h"

#endif /* __PM_H__ */
