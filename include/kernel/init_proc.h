#ifndef __TRAPFRAME_H__
#define __TRAPFRAME_H__

#include "kernel/process.h"

PCB* create_kthread(void*);

PCB* create_kthread_with_args(void*, int);

void set_pdir(PCB*, uint32_t);

#endif /* __TRAPFRAME_H__ */