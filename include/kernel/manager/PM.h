#ifndef __PM_H__
#define __PM_H__

#include "common.h"
#include "kernel/message.h"

enum {
    PM_CREATE,
    PM_fork,
};

void init_PM();
void create_process(Msg*);

extern int PM;

#include "PM_pdir.h"

#endif /* __PM_H__ */
