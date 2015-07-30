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
PCB * kfork(Msg* m) {
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
    // initialize link
    list_init(&(child->link));
    // initialize waiting list
    list_init(&child->waitpid);

    // share virtual address storage
    list_init(&(child->vir_mem));
    list_copy(&(father->vir_mem), Seg_info, link);

    return child;
}

int free_process(PCB *aim) {
    Msg m;
    // free page, then page table, then page directory
    init_msg(
        &m,
        current->pid,
        FREE_page,
        INVALID_ID, INVALID_ID, aim, INVALID_ID, INVALID_ID);
    pdir_free(get_pdir_addr(aim));
    // free message queue
    list_free(&aim->mes, Msg, list);
    // free virtual address space
    list_free(&(aim->vir_mem), Seg_info, link);
    // free waitpid
    list_free(&aim->waitpid, PCB, link);
    // free pcb
    kfree(aim);
    return 1;

}

int save_args(Msg *m, char *buf) {
    switch(m->type) {
        case PM_exec:
            /**
               user stack starting state of:
               int main(char *args)

               ______________KERNEL_VA_START -- 0xc0000000
               |          |
               |   ....   |
               |  *args   |
               |__________|
               |___args___|
               |___eip ___|
               |___ebp ___|

               int main(int argc, char *argv[]) -- not implemented now

               ______________KERNEL_VA_START -- 0xc0000000
               |          |
               |   ....   |
               | *argv[n] |
               |__________|
               |   ....   |
               |__________|
               |          |
               |   ....   |
               | *argv[1] |
               |__________|
               |          |
               |   ....   |
               | *argv[0] |
               |__________|
               |_argv[n]__|
               |   ....   |
               |_argv[1]__|
               |_argv[0]__|
               |___argc___|
               |___eip ___|
               |___ebp ___|

            */
        {
            char *src = get_pa(&((PCB *)m->i[1])->pdir, (uint32_t)m->buf);
            *(uint32_t *)buf = (uint32_t)src;
            size_t len = strlen(src) + 1;
            memcpy(buf + sizeof src, src, len);
            return len + sizeof src;
        }
        default:
            printk(RED"not implemented now"RESET);
            assert(0);
            break;
    }
}

#define BUF_SZ 512

PCB * kexec(Msg *m) {
    char args[BUF_SZ];
    PCB *aim = (PCB *)m->i[1];
    // save arguments
    size_t len = save_args(m, args);
    // save the resources to inherit: pid, file descriptor
    pid_t p = aim->pid;
    // file descriptor
    // free process
    free_process(aim);
    // create a new one
    PCB *new = create_process(m);
    new->pid = p;
    pid_count_des();
    // prepare args on the stack
    // push args *
    memcpy(user_stack(new) - len, args, len);
    // TODO `- sizeof(int)` for saved eip
    set_esp(new, USER_STACK_BASE - len - sizeof(int));
    return new;
}

void notify_wait(PCB *aim) {
    ListHead *p = NULL;
    ListHead *head = &aim->waitpid;
    PCB *t = NULL;
    Msg m;
    m.src = current->pid;
    // set return value for waitpid
    m.ret = 1;
    list_foreach(p, head) {
        t = list_entry(p, PCB, link);
        send(t->pid, &m);
    }
}

int kexit(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    notify_wait(aim);
    return free_process(aim);
}

/**
   Store the wait to target process
 */
void kwaitpid(Msg *m) {
    PCB *aim = fetch_pcb(m->i[0]);
    list_add_after(&aim->waitpid, m->buf);
}