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

void copy_kstack(PCB *father, PCB *child) {
    // @checked size: [tf, (char *)father->kstack + KSTACK_SIZE)
    uint32_t copy_size = (char *)father->kstack + KSTACK_SIZE - (char *)father->tf;
    // allocate trapframe, and function invoke stack to the end of stack
    void *second_frame = (void *)((char *)(child->kstack) + KSTACK_SIZE - copy_size);
    // @see user_process_fork.jpg: father's tf is now point to the second TrapFrame
    memcpy(second_frame, father->tf, copy_size);
    child->tf = second_frame;
    // handle the content on the kernel stack, ebp, esp
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
    int32_t gap = father->kstack - child->kstack;
    // handle epb, esp in the TrapFrame
    TrapFrame * s_frame = ((TrapFrame *)second_frame);
    // first change it to point to right place, store the pointer to next ebp
    s_frame->ebp += gap;
    s_frame->xxx += gap;
    uint32_t* ebp = (uint32_t *)s_frame->ebp;
    while ((uint32_t)ebp >= KERNEL_VA_START) {
        *ebp += gap; // TODO check addition of uint32_t and int32_t
        ebp = (uint32_t *)*ebp;
        assert(*ebp > (uint32_t)ebp);
    }
    // 4 bytes for ebp, 4 bytes for `call` pushed eip, 4 bytes for `push %esp` esp
    /* The pointer in first TrapFrame point to the
       user stack, and that are same for all user process, so
       no need to change it
    TrapFrame *f_frame = ((TrapFrame *)(char *)&ebp - 4 - 4 - 4);
    f_frame->ebp += gap;
    f_frame->esp += gap;
    */
}

void copy_user_stack(PCB *father, PCB *child) {
    void *fa_start = (void *)get_pa(&father->pdir, USER_STACK_POINTER);
    void *child_start = (void *)get_pa(&child->pdir, USER_STACK_POINTER);
    memcpy(child_start, fa_start, USER_STACK_SIZE);
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
    // deep copy ptable, page, set page directory
    init_msg(m,
        current->pid,
        COPY_page,
        (int)father, (int)child, NULL, INVALID_ID, INVALID_ID);
    send(MM, m);
    receive(MM, m);
    // special handle kernel stack and tf
    copy_kstack(father, child);

    // handle content on the user stack which must use physical address to **copy**
    // for current thread is PM which has different page directory with user process
    // The pointer field on the user stack is the same, so no need to change pointer field.
    if (father->type == USER) {
        copy_user_stack(father, child);
    }

    // clear message queue
    list_init(&(child->mes));
    list_init(&(child->link));

    // share virtual address storage
    list_init(&(child->vir_mem));
    list_add_after(&(child->vir_mem), father->vir_mem.next);

    //reply message
    m->ret = child->pid;
    m->src = current->pid;
    assert(current->pid == PM);
    send(fa, m);
    // reply to child
    m->ret = 0;
    m->src = current->pid;
    assert(current->pid == PM);
    send(child, m);
}
