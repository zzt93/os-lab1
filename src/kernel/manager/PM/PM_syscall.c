#include "kernel/manager/PM_syscall.h"
#include "kernel/manager/manager.h"
#include "kernel/process.h"
#include "lib/string.h"
#include "lib/malloc.h"

#include "kernel/manager/fd.h"

static void s_copy(PCB *src, PCB *dest) {
    dest->state = src->state;
    dest->count_of_lock = src->count_of_lock;
    dest->type = src->type;
    assert(src->type == USER);
    dest->pid = new_id();
    assert(dest->pid > src->pid);
    int i;
    for (i = 0; i < PROCESS_MAX_FD; i++) {
        assign_fd(dest->fd_table + i, src->fd_table + i);
    }
    dest->priority = src->priority;
}

/**
   I. copy full kernel stack, include two TrapFrame:
     In this choice, I have to change all the pointer field which
     point to kernel stack, including function stack trace(ebp,
     local variables's address) and xxx, ebp in TrapFrame

   II. copy just the first TrapFrame:
     In this occasion, only change xxx, ebp is fine
 */
// TODO work for only user thread to fork??
void copy_kstack(PCB *father, PCB *child) {
    // handle the content in the first TrapFrame, ebp, xxx
    TrapFrame *s_frame = ((TrapFrame *) father->tf);
    uint32_t *ebp = (uint32_t *) s_frame->ebp;
    while (*ebp >= KERNEL_VA_START) {
        assert(*ebp > (uint32_t) ebp);
        ebp = (uint32_t *) *ebp;
    }
    // 4 bytes for ebp, 4 bytes for `call` pushed eip, 4 bytes for `push %esp` esp
    /* The ebp in first TrapFrame point to the
       user stack, and that are same for all user process, so
       no need to change it
    f_frame->ebp += gap;
    */
    int32_t gap = child->kstack - father->kstack;
    TrapFrame *child_frame = (TrapFrame *) ((char *) ebp + 4 + 4 + 4 + gap);
    assert((char *) child_frame - gap == (char *) father->kstack + KSTACK_SIZE - sizeof(TrapFrame));
    uint32_t copy_size = sizeof(TrapFrame);
    memcpy(child_frame, (char *) child_frame - gap, copy_size);
    child_frame->xxx += gap;
    // set return value of fork
    child_frame->eax = 0;
    child->tf = child_frame;
    // for it is occasion two, count_of_lock should be change
    child->count_of_lock--;
}

// this function can't use until finish the conversion
// of every parameter which is address
void copy_kstack_full(PCB *father, PCB *child) {
    // @checked size: [tf, (char *)father->kstack + KSTACK_SIZE)
    uint32_t copy_size = (char *) father->kstack + KSTACK_SIZE - (char *) father->tf;
    // this assert is not so accurate for
    // 1. I ignore the function backtrace
    // 2. The second TrapFrame hasn't push `ss` and `esp`
    assert(copy_size > 2 * sizeof(TrapFrame));
    assert((uint32_t) father->tf < (uint32_t) father->kstack + KSTACK_SIZE
                                   && (uint32_t) father->tf > (uint32_t) father->kstack);
    // allocate trapframe, and function invoke stack to the end of stack
    void *second_frame = (void *) ((char *) (child->kstack) + KSTACK_SIZE - copy_size);
    // @see ram_user_process_fork.jpg: father's tf is now point to the second TrapFrame
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
    int32_t gap = child->kstack - father->kstack;
    // handle epb, esp in the later TrapFrame
    TrapFrame *s_frame = ((TrapFrame *) second_frame);
    // first change it to point to right place, store the pointer to next ebp
    s_frame->ebp += gap;
    s_frame->xxx += gap;
    uint32_t *ebp = (uint32_t *) s_frame->ebp;
    while (*ebp >= KERNEL_VA_START) {
        // @checked addition of uint32_t and int32_t
        // addition is safe, but compare is dangerous
        *ebp += gap;
        assert(*ebp > (uint32_t) ebp);
        ebp = (uint32_t *) *ebp;
    }
    // 4 bytes for ebp, 4 bytes for `call` pushed eip, 4 bytes for `push %esp` esp
    /* The ebp in first TrapFrame point to the
       user stack, and that are same for all user process, so
       no need to change it
    TrapFrame *f_frame = ((TrapFrame *)(char *)&ebp + 4 + 4 + 4);
    f_frame->ebp += gap;
    f_frame->xxx += gap;
    */
    // change every parameter which is address
    // of every function call on the kernel stack
}

/*
  This function is somewhat wrong, for if the user stack
  is larger than a page, it may be not consecutive region
  in physical address, so just copy from start is wrong.
void copy_user_stack(PCB *father, PCB *child) {
    void *fa_start = user_stack_pa(father, USER_STACK_POINTER);
    void *child_start = user_stack_pa(child, USER_STACK_POINTER);
    memcpy(child_start, fa_start, USER_STACK_SIZE);
}
*/

/**
   TODO check whether every field in pcb is correctly copied
   @see init_proc.c; process.h
 */
// return the pid of child process
PCB *kfork(Msg *m) {
    PCB *father = m->buf;
    PCB *child = kmalloc(PCB_SIZE);
    // shallow copy: privilege, sign, pid, fd_table
    s_copy(father, child);
    // deep copy ptable, page, set page directory
    // (including user stack, except kernel image)
    // To handle content on the user stack, we must use physical address to **copy**
    // because current thread is PM which has different page directory with user process
    // The pointer field on the user stack is the same, so no need to change pointer field.
    // It will re-use read-only page which save memory and time: for example, code segment
    // and read-only data.
    init_msg(m,
             current->pid,
             COPY_page,
             (int) father, (int) child, NULL, INVALID_ID, INVALID_ID);
    send(MM, m);
    receive(MM, m);

    // TODO user process need kernel image???
    init_kernel_image(get_pdir_addr(child));
    // special handler for kernel stack and tf
    // copy just the first TrapFrame:
    // In this occasion, only change xxx, ebp is fine
    copy_kstack(father, child);


    // clear message queue
    list_init(&(child->mes));
    // initialize link
    list_init(&(child->link));
    // initialize waiting list
    list_init(&child->waitpid);

    // copy virtual address storage
    list_init(&(child->vir_mem));
    list_copy(&(father->vir_mem), Seg_info, link, &child->vir_mem);

    // copy cwd_path
    child->cwd_path = NULL;
    set_cwd_path(child, father->cwd_path);
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
    // free waiting list
    list_free(&(aim->waitpid), Waiting, link);
    // free cwd_path
    free_cwd_path(aim);

    // before invalidate the reference of PCB, release the reference
    // in the sleeped_tree -- for process call `exit` and `exec` are
    // are sleeping on waiting PM's message
    delete_ref(aim);
    // return pid
    pid_free(aim->pid);
    // free pcb
    kfree(aim);
    return 1;

}

int save_args(Msg *m, char *buf) {
    switch (m->type) {
        case PM_exec:
            /**
               user stack state at the beginning:
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
            // for kernel can read the address of user process,
            // so just read it in user memory and not copy it to
            // kernel
            char *src = get_pa(&((PCB *) m->i[1])->pdir, (uint32_t) m->buf);
            // just take a place of address, not real address
            *(uint32_t *) buf = 0;
            size_t len = strlen(src) + 1;
            memcpy(buf + sizeof src, src, len);
            size_t all = ALIG(len + sizeof src);
            // set real address
            *(uint32_t *) buf = USER_STACK_BASE - all + sizeof src;
            return all;
        }
        default:
            printk(RED"not implemented now"RESET);
            assert(0);
            break;
    }
}

#define ARGS_MAX_LEN 512

/**
   In my current implementation, pid is not necessarily the
   same for the process before exec and after it
   -- TODO add lock if necessary
 */
PCB *kexec(Msg *m) {
    char args[ARGS_MAX_LEN] = {0};
    PCB *aim = (PCB *) m->i[1];
    // TODO check whether the file is executable before free current process

    // save arguments if necessary
    size_t len;
    assert(m->buf != NULL);
    len = save_args(m, args);
    // save the resources to inherit: pid, file descriptor, cwd_path
    FDE tmp[PROCESS_MAX_FD];
    memcpy(tmp, aim->fd_table, sizeof(FDE) * PROCESS_MAX_FD);
    int path_len = strlen(aim->cwd_path) + 1;
    char tmp_path[path_len];
    memcpy(tmp_path, aim->cwd_path, path_len);

    // free process
    free_process(aim);
    // create a new one from file
    PCB *new = create_process(m);
    if (new == NULL) {
        return NULL;
    }
    memcpy(new->fd_table, tmp, sizeof(FDE) * PROCESS_MAX_FD);
    set_cwd_path(new, tmp_path);
    // prepare args on the stack
    // push args *
    memcpy(user_stack_pa(new, USER_STACK_BASE - len), args, len);
    // `- sizeof(int)` for saved eip
    set_esp(new, USER_STACK_BASE - len - sizeof(int));
    return new;
}

void notify_wait(PCB *aim) {
    ListHead *p = NULL;
    ListHead *head = &aim->waitpid;
    Waiting *t = NULL;
    Msg m;
    m.src = current->pid;
    printk("notify wait #%d\n", m.src);
    // set return value for waitpid
    m.ret = SUCC;
    list_foreach(p, head) {
        t = list_entry(p, Waiting, link);
        send(t->wait->pid, &m);
    }
}

int kexit(Msg *m) {
    PCB *aim = (PCB *) m->buf;
    notify_wait(aim);
    int i;
    for (i = 0; i < PROCESS_MAX_FD; i++) {
        m->dev_id = i;
        close_file(m);
    }
    return free_process(aim);
}

/**
   Store the waiting process to target process
 */
void kwaitpid(Msg *m) {
    PCB *aim = fetch_pcb(m->i[0]);
    if (aim == NULL) {
        m->src = current->pid;
        m->ret = FAIL;
        send(((PCB *) m->buf)->pid, m);
        return;
    }
    Waiting *w = kmalloc(sizeof(Waiting));
    init_wait(w, m->buf);
    // let m->buf, ie user process waiting on the aim
    list_add_after(&aim->waitpid, &w->link);
    printk("waitpid #%d\n", m->i[0]);
}