#ifndef __PM_SYSCALL_H__
#define __PM_SYSCALL_H__

#include "kernel/message.h"

int kfork(Msg *);
int kexec(Msg *);
int kexit(Msg *);


#endif /* __PM_SYSCALL_H__ */