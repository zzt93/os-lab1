#ifndef __PM_H__
#define __PM_H__

#include "common.h"
#include "kernel/message.h"

enum {
    PM_CREATE,
};

void init_PM();
void create_process(Msg*);

extern int PM;

#endif /* __PM_H__ */