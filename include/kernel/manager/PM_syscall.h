#ifndef __PM_SYSCALL_H__
#define __PM_SYSCALL_H__

#include "kernel/message.h"
#include "kernel/process.h"

#include "adt/list.h"

PCB *kfork(Msg *);

PCB *kexec(Msg *);

int kexit(Msg *);

void kwaitpid(Msg *);

typedef struct {
    ListHead link;
    PCB *wait;
} Waiting;

static inline void
init_wait(Waiting *w, PCB *p) {
    list_init(&w->link);
    w->wait = p;
}

#endif /* __PM_SYSCALL_H__ */