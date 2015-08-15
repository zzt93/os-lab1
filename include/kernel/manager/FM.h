#ifndef __FM_H__
#define __FM_H__

#include "common.h"
#include "kernel/message.h"

enum {
    FM_read,
    FM_write,
    FM_open,
    FM_close,
    FM_dup,
    FM_dup2,
    FM_lseek,
};

void init_FM();
void read_file(Msg*);

extern int FM;

extern uint32_t init_off;

#include "fd.h"

#endif /* __FM_H__ */