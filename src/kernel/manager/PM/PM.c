#include "kernel/process.h"
#include "kernel/init_proc.h"
#include "kernel/message.h"

#include "kernel/manager/manager.h"

#include "kernel/memory.h"

#include "kernel/manager/PM_syscall.h"

#include "lib/malloc.h"

#define B_SIZE 512

extern PCB *current;

int PM;

#include "kernel/user_process.h"
#include "kernel/manager/f_dir.h"

void change_to_bin() {
    set_cwd_path(current, default_cwd_name);
    Msg m;
    m.buf = current;
    m.dev_id = (int) bin;
    ch_dir(&m);
}

/**
   The message sent to PM should specify:
   m->type
       @type:PM_fork
       m->buf -- the father's PCB's address
       @return:
       m->ret -- the child pid

       @type:PM_exec
       m->i[0] -- the program/file name
       m->buf -- address of string
       @return: Returns success unless COMMAND is not found or a redirection error occurs.
       m->ret -- the status of execution
*/
static void PM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        pid_t dest = m.src;
        switch (m.type) {
            /**
               @param m->i[0] -- (int)the program/file fd
               @return: The message send from PM specify:
               m->ret -- whether successfully create a user process
            */
            case PM_CREATE: {
                // change the cwd to /bin
                change_to_bin();

                Msg open_m;
                open_m.req_pid = (int) current;
                open_m.buf = current;
                open_m.dev_id = m.i[0];
                // set file descriptor for create_process
                int fd = open_file(&open_m);
                assert(fd > CWD && fd < PROCESS_MAX_FD);
                m.i[0] = fd;

                PCB *p = create_process(&m);
                if (p == NULL) {
                    m.ret = FAIL;
                } else {
                    add2wake(p);
                    m.ret = SUCC;
                }
                // close file anyway
                Msg close_m;
                close_m.buf = current;
                close_m.dev_id = fd;
                close_file(&close_m);
                break;
            }
            case PM_fork: {
                PCB *new = kfork(&m);
                assert(new != NULL);
                assert(new->state > NOT_SLEEPED);
                new->state -= SLEEPED;
                lock();
                put_by_state(new);
                add_process(new);
                unlock();
                pid_t child = new->pid;
                // reply to father
                m.ret = child;
                break;
            }
            case PM_exec: {
                Msg open_m;
                open_m.req_pid = m.i[1];
                open_m.buf = current;
                open_m.dev_id = m.i[0];
                // set file descriptor for exec
                int fd = open_file(&open_m);
                m.i[0] = fd;
                if (invalid_fd_i(m.i[0])) {
                    m.ret = NOT_EXE;
                    break;
                }

                /**
                   @param m->i[0] -- (int) fd
                   @param m->i[1] -- (PCB *) the process to exec itself
                   @param m->buf -- (char *)args address
                 */
                PCB *new = kexec(&m);
                if (new != NULL) {// mean exec succeed
                    // put in queue
                    add2wake(new);
                }
                // close file anyway
                Msg close_m;
                close_m.buf = current;
                close_m.dev_id = fd;
                close_file(&close_m);
                // the thread invoke `exec()` will execute different code,
                // i.e. will not wait to receive response,
                // so no need to send response message back
                continue;
            }
            case PM_ram_exec: {
                PCB *new = kexec(&m);
                if (new != NULL) {
                    // put in queue
                    add2wake(new);
                }
                // the thread invoke `exec()` will execute different code,
                // i.e. will not wait to receive response,
                // so no need to send response message back
                continue;
            }
            case PM_exit:
                kexit(&m);
                // the thread invoke `exit` will not continue to run,
                // so no need to send back
                continue;
            case PM_waitpid:
                kwaitpid(&m);
                continue;
            default:
                assert(false);
                break;
        }
        m.src = current->pid;
        send(dest, &m);
    }
}

void init_PM() {
    PCB *p = create_kthread(PM_job);
    PM = p->pid;
    add2wake(p);
}

// initialize the va for stack
// set the page directory, page table for user stack
// and allocate physical page is necessary
void create_va_stack(PDE *pdir, uint32_t *ss, uint32_t *esp) {
    unsigned char *va = (unsigned char *) USER_STACK_POINTER;
    assert(va == (unsigned char *) 0xbffff000);
    Msg m;
    init_msg(&m,
             current->pid,
             NEW_PAGE,
             INVALID_ID,
            // make a user writable page
             (USER_PAGE_ENTRY << 2) | (PAGE_W << 1),
             pdir, (int) va, USER_STACK_SIZE);

    send(MM, &m);
    receive(MM, &m);
    // TODO which segment, although all segment are
    // the same for time being
    *ss = SELECTOR_USER(SEG_USER_DATA);
    *esp = (uint32_t) (va + USER_STACK_SIZE);
    assert(*esp == KOFFSET);
}

/**
   user stack initial page end position -- must minus some number
   for it is out of range
   user stack initial page starting position
 */
void *user_stack_pa(PCB *p, uint32_t val) {
    return get_pa(&p->pdir, val);
}


/**
   single responsibility:
   just create process,
   not add it to wake queue
   not send message back
 */
PCB *create_process(Msg *m) {
    int name = m->i[0];
    assert(name >= 0);
    char buf[B_SIZE];

    // create page directory for new process
    // must using physical address because cr3 should only store pa
    PDE *pdir = pdir_alloc();
    assert(((int) pdir & 0xfff) == 0);
    /* read 512 bytes starting from offset 0 from file "name"
       into buf */
    /* it contains the ELF header and program header table */
    init_msg(m,
             current->pid,
             FM_read,
             (int) current, name, buf, 0, B_SIZE);
    send(FM, m);
    receive(FM, m);
    if (m->ret != SUCC) {
        return NULL;
    }

    struct ELFHeader *elf = (struct ELFHeader *) buf;
    struct ProgramHeader *ph_table, *end_ph;
    unsigned char *va, *pa, *i;
    //store virtual address space in loop
    ListHead vir_range;
    list_init(&vir_range);

    ph_table = (struct ProgramHeader *) ((char *) elf + elf->phoff);
    // ignore the stack header for the time being
    end_ph = ph_table + elf->phnum - 1;
    for (; ph_table < end_ph; ph_table++) {
        /* scan the program header table, load each segment into memory */

        va = (unsigned char *) ph_table->vaddr; /* virtual address */
        //assert(va >= 0x08048000);
        assert(va > (unsigned char *) 0);
        assert(va < (unsigned char *) KERNEL_VA_START);
        /*
          store the information about virtual address
         */
        Seg_info *tmp = kmalloc(sizeof(Seg_info));
        vir_init(tmp,
                 (uint32_t) va, (uint32_t) va + ph_table->memsz, ph_table->flags);
        list_add_before(&vir_range, &(tmp->link));
        /* allocate pages starting from va, with memory size no less than ph->memsz */
        /*
          flags: RWE is the lowest three bits
          TODO: is always user? i.e. always use USER_PAGE_ENTRY?
         */
        init_msg(m,
                 current->pid,
                 NEW_PAGE,
                 INVALID_ID, (ph_table->flags & 0x2) | (USER_PAGE_ENTRY << 2), pdir, (int) va, ph_table->memsz);
        send(MM, m);
        receive(MM, m);

        pa = m->buf; // pa a is physical address
        // MM should already set the mapping from va to pa,
        // but the pdir is not this process's page directory,
        // using it can't find the right physical address,
        // so have to use physical address directly
        assert(pa >= (unsigned char *) KERNEL_PA_END);
        assert(pa < (unsigned char *) PHY_MEM);
        /* read ph->filesz bytes starting from offset ph->off from file into pa */
        if (ph_table->filesz != 0) {
            // change the offset for disk read -- disk read ignore direct offset parameter
            m->buf = current;
            m->dev_id = name;
            m->len = SEEK_SET;
            m->offset = ph_table->off;
            // change offset and read should be atomic operation
            lseek_file(m);
            init_msg(m,
                     current->pid,
                     FM_read,
                     (int) current, name, pa, ph_table->off, ph_table->filesz);
            // old ram way
            // INVALID_ID, name, pa, ph_table->off, ph_table->filesz);
            send(FM, m);
            receive(FM, m);
        }
        // initialize the gap between [file_size, memory_size)
        // all to zero
        for (i = pa + ph_table->filesz; i < pa + ph_table->memsz; *i++ = 0);
    }

    // initialize the va for stack
    // set the page directory, page table for user stack
    // and allocate page
    uint32_t ss = 0;
    uint32_t esp = 0;
    create_va_stack(pdir, &ss, &esp);

    // initialize the va for kernel image
    init_kernel_image(pdir);

    // initialize PCB for user process
    void *f = (void *) elf->entry;
    // old way: -- now replaced by create_user_thread
    //PCB* p = create_kthread(f);
    //set_pdir(p, (uint32_t)pdir);
    //set_user_tf(p, ss, esp);
    PCB *p = create_user_thread(f, (uint32_t) pdir, ss, esp, &vir_range, default_cwd);
    return p;
}