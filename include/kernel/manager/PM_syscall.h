#ifndef __PM_SYSCALL_H__
#define __PM_SYSCALL_H__

#include "kernel/message.h"
#include "kernel/process.h"

PCB * kfork(Msg *);
PCB * kexec(Msg *);
int kexit(Msg *);


#endif /* __PM_SYSCALL_H__ */