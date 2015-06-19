#ifndef __TRAPFRAME_H__
#define __TRAPFRAME_H__

#include "kernel/process.h"

PCB* create_kthread(void*);

PCB* create_kthread_with_args(void*, int);

//void set_pdir(PCB*, uint32_t);
//void set_user_tf(PCB*, uint32_t ss, uint32_t esp);

PCB* create_user_thread(
    void *f, uint32_t pdir, uint32_t ss, uint32_t esp);

int new_id();

static inline void set_pdir(PCB* p, uint32_t addr) {
    assert((addr&0xfff) == 0);
    p->pdir.val = 0;
    p->pdir.page_directory_base = addr >> 12;
}


/**
   set the tf store the content of user stack
   for user process running on it
 */
static inline void set_user_stack(PCB* p, uint32_t ss, uint32_t esp) {
    TrapFrame* frame = (TrapFrame*)p->tf;
    frame->ss = ss;
    frame->esp = esp;
}

#endif /* __TRAPFRAME_H__ */