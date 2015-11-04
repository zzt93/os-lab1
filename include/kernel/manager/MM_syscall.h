#ifndef __MM_SYSCALL_H__
#define __MM_SYSCALL_H__

#include "kernel/message.h"

int page_copy(Msg *m);
int page_free(Msg *m);

#endif /* __MM_SYSCALL_H__ */