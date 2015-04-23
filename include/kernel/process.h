#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"
#define KSTACK_SIZE 4096

typedef struct {
    void *tf;
    uint8_t kstack[KSTACK_SIZE];
    ListHead link;
} PCB;

extern PCB *current;

#endif
