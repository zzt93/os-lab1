#ifndef __MM_H__
#define __MM_H__

#include "kernel/elf.h"
#include "kernel/message.h"

enum {
    NEW_PAGE,
    NEW_PDIR,
};

void init_MM();
void init_va(Msg*);

extern int MM;

#include "MM_alloc.h"

#endif /* __MM_H__ */
