#ifndef __TRAPFRAME_H__
#define __TRAPFRAME_H__

#include "kernel/process.h"
#include "kernel/manager/fd_ft.h"

PCB* create_kthread(void*);

PCB* create_kthread_with_args(void*, int);

//void set_pdir(PCB*, uint32_t);
//void set_user_tf(PCB*, uint32_t ss, uint32_t esp);

PCB* create_user_thread(
    void *f,
    uint32_t pdir,
    uint32_t ss, uint32_t esp,
    ListHead *vir,
    FTE *cwd);

int new_id();
void set_esp(PCB *p, uint32_t esp);
void pid_free();

static inline
void set_pdir(PCB* p, uint32_t addr) {
    assert((addr&0xfff) == 0);
    p->pdir.val = 0;
    p->pdir.page_directory_base = addr >> 12;
}


/**
   set the tf store the content of user stack
   for user process running on it
 */
static inline
void set_user_stack(PCB* p, uint32_t ss, uint32_t esp) {
    TrapFrame* frame = (TrapFrame*)p->tf;
    frame->ss = ss;
    frame->esp = esp;
}

static inline
void set_cwd(PCB *pcb, FTE *fte) {
    assign_fte(&pcb->fd_table[CWD], fte);
}

void init_fd_table(PCB *pcb, FTE *cwd);

static inline
int priority_in_range(Pri_t priority) {
    return priority >= 0 && priority < 65536;
}

void kset_edf_priority(PCB *p, Pri_t priority);

static inline
Pri_t kget_priority(PCB *p) {
    return p->priority;
}


void set_cwd_path(PCB *pcb, const char *name);
void append_cwd_path(PCB *pcb, const char *name);
void free_cwd_path(PCB *pcb);


#define DEFAULT_STATE WAKED

#endif /* __TRAPFRAME_H__ */