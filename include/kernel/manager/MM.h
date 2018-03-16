#ifndef __MM_H__
#define __MM_H__

#include "kernel/elf.h"
#include "kernel/message.h"

enum {
    NEW_PAGE,
    NEW_PDIR,
    COPY_page,
    FREE_page,
};

void init_MM();

int init_va(Msg *);

extern int MM;

#include "MM_alloc.h"
#include "MM_syscall.h"
#include "MM_util.h"

#endif /* __MM_H__ */
