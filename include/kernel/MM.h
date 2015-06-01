#ifndef __MM_H__
#define __MM_H__

#include "elf.h"
#include "kernel/message.h"

enum {
    NEW_PAGE,
};

void init_MM();
void alloc_page(Msg*);

extern int MM;

#endif /* __MM_H__ */
