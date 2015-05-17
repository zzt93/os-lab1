#ifndef __TRAPFRAME_H__
#define __TRAPFRAME_H__

PCB* create_kthread(void*);

PCB* create_kthread_with_args(void*, int);

#endif /* __TRAPFRAME_H__ */