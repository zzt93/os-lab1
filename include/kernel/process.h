#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"
#include "kernel/semaphore.h"
#include "x86/cpu.h"

#include "manager/Virtual_mem.h"

#define KSTACK_SIZE 4096
#define PCB_SIZE (sizeof (PCB))

typedef enum {
    IDLE, WAKED,
    EDF,
    // add new state before this
            NOT_SLEEPED,
    // always be the last
            SLEEPED = 100,
} PROCESS_STATE;

typedef enum {
    KERNEL, USER,
} Thread_t;

#include "kernel/manager/fd.h"

#define PROCESS_MAX_FD 8

static inline
bool invalid_fd_i(int fd) {
    return fd >= PROCESS_MAX_FD || fd < 0;
}


/*
  for the stack is grow to small address,
  the first item on the stack, should be the
  last in the struct

  在asm_do_irq中, 代码将会把A的通用寄存器保存到A的堆栈上,
  这些寄存器的内容连同之前保存的#irq和
  硬件保存的EFLAGS, CS, EIP形成了"陷阱帧"(trap frame)的数据结构
 */
/**
   Every time you add a new field in PCB, you have to check the following method:
   - init_idle
   - init_pcb_content
   - kfork
   - free_process
 */
typedef struct {
    /**
       void *tf must be the first item of PCB? @see do_irq.S
       TrapFrame store the thread context info @see memory.h
     */
    void *tf;

    // stack of user
    uint8_t kstack[KSTACK_SIZE];
    // pid
    pid_t pid;
    PROCESS_STATE state;
    // to be linked on a semaphore list by P() & V()
    // @invariant: if it is running, link must be empty
    ListHead link;
    ListHead mes;
    //Sem mes_lock;
    int count_of_lock;

    CR3 pdir;

    Thread_t type;
    // link to Seg_info
    ListHead vir_mem;
    // link to waiting process by link
    ListHead waitpid;
    // file descriptor table
    struct file_descriptor_entry fd_table[PROCESS_MAX_FD];
    // process priority -- used when schedule process
    Pri_t priority;
    // cwd path for user process
    char *cwd_path;
    // the pid the process waiting for
    //pid_t waiting_for;
} PCB;

extern PCB *current;

// implemented in schedule.c
void add2wake(PCB *);

void add2sleeped(PCB *);

void sleep();

void sleep_to(ListHead *, void (*)(ListHead *, PCB *));

void wake_up_from(ListHead *, PCB *(*)(ListHead *));

void wake_up_lock(PCB *, int);

void wake_up(PCB *);

void delete_ref(PCB *);

// implement in process.c
PCB *fetch_pcb(int);

void add_process(PCB *);

int remove_process(PCB *pcb);

size_t pcb_size();

int fetch_all_pcb(PCB **pcbs, int capacity);

static inline void *get_pdir_addr(PCB *p) {
    return (void *) (p->pdir.page_directory_base << 12);
}

static inline
int kusing_edf() {
    current->state = EDF;
    return 1;
}

#define IDLE_ID 0
#define START_ID (IDLE_ID+1)
#define PID_LIMIT 4096

// for message
#define ANY -1

extern Sem wake_lock, sleeped_lock;

// for priority
#define KERNEL_PRI 100
#define USER_PRI 10


void put_by_state(PCB *);

#define INVALID_PID (PID_LIMIT+1)

//#define CWD_PATH_LENGTH 32

#include "init_proc.h"

#endif
