#ifndef __FM_H__
#define __FM_H__

#include "common.h"
#include "kernel/message.h"

enum {
    FM_READ,
};

void init_FM();
void read_file(Msg*);

extern int FM;
#endif /* __FM_H__ */