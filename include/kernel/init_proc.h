#ifndef __TRAPFRAME_H__
#define __TRAPFRAME_H__

#include "kernel/process.h"

PCB* create_kthread(void*);

PCB* create_kthread_with_args(void*, int);

//void set_pdir(PCB*, uint32_t);
//void set_user_tf(PCB*, uint32_t ss, uint32_t esp);

PCB* create_user_thread(
    void *f, uint32_t pdir, uint32_t ss, uint32_t esp);

#endif /* __TRAPFRAME_H__ */