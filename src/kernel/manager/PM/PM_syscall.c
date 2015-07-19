#include "kernel/manager/PM_syscall.h"
#include "kernel/manager/manager.h"
#include "kernel/process.h"
#include "lib/string.h"
#include "lib/malloc.h"

static void s_copy(PCB* src, PCB* dest) {
    dest->state = src->state;
    dest->count_of_lock = src->count_of_lock;
    dest->type = src->type;
    assert(src->type == USER);
    dest->pid = new_id();
    assert(dest->pid > src->pid);
}

/**
   TODO check whether every field in pcb is correctly copied
   @see init_proc.c; process.h
 */
// return the pid of child process
void kfork(Msg* m) {
    pid_t fa = m->src;
    PCB *father = m->buf;
    PCB *child = kmalloc(PCB_SIZE);
    // shallow copy: privilege, sign, pid
    s_copy(father, child);
    // deep copy ptable, page
    init_msg(m,
        current->pid,
        COPY_page,
        (int)father, (int)child, NULL, INVALID_ID, INVALID_ID);
    send(MM, m);
    receive(MM, m);
    // special handle tf
    TrapFrame *frame = (TrapFrame*)((char *)(child->kstack) + KSTACK_SIZE - sizeof(TrapFrame)); // allocate frame at the end of stack
    // @see user_process_fork.jpg: father's tf is now point to the second TrapFrame
    memcpy(frame, father->tf, sizeof(TrapFrame));
    child->tf = frame;
    // handle the content on the kernel stack
    /*
      至于内核栈上由于函数调用而保存的ebp值, 我们观察到函数调用保存的ebp值会形成一条函数调用链:
      tf->ebp 指向异常现场所在函数的栈帧, *(tf->ebp)指向上一层函数的栈帧,
      **(tf->ebp)指向上上层函数的栈帧..., when we meet a ebp == 0
      which is the default value when initialize TrapFrame
      and then pop to ebp,
      and then pushed by the function start by 'push ebp'
      我们只需要从tf->ebp开始, 把函数调用链上的每一个ebp 加上父子进程内核栈的相对偏移量,
      子进程将来就可以正确地从异常和函数中返回了.
     */
    //int32_t gap = father->kstack - child->kstack;
    uint32_t ebp = ((TrapFrame*)father->tf)->ebp;
    while (ebp != 0) {
    }

    //reply message
    m->ret = child->pid;
    m->src = current->pid;
    assert(current->pid == PM);
    send(fa, m);
}
