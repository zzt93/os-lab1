#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"
#include "kernel/semaphore.h"
#include "x86/cpu.h"

#include "manager/Virtual_mem.h"
#define KSTACK_SIZE 4096
#define PCB_SIZE (sizeof (PCB))

typedef enum {
    IDLE, SLEEPED, WAKED,
} PROCESS_STATE;

typedef enum {
    KERNEL, USER,
} Thread_t;

#include "kernel/manager/fd.h"

#define PROCESS_MAX_FD 8


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
       void *tf must be the first item of PCB,
       see do_irq.S
     */
    void *tf; // the value is esp, pushl esp before call irq_handle
    /* the content in the trapFrame, ie *tf
    // context info
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t gs, fs, ed, ds;
    int irq;
    int error_code;
    unsigned int eip;
    uint32_t cs;
    unsigned int eflags;
    */
    // stack of user
    uint8_t kstack[KSTACK_SIZE];
    // pid
    int pid;
    PROCESS_STATE state;
    // to be linked on a semaphore list
    // if it is running, link must be empty
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
} PCB;

extern PCB *current;

// implemented in schedule.c
void add2wake(PCB*);
void add2sleeped(PCB*);
void sleep();
void sleep_to(ListHead*, void (*)(ListHead*, PCB*));
void wake_up_from(ListHead*, PCB* (*)(ListHead*));
void wake_up_lock(PCB*, int);
void wake_up(PCB*);
void delete_ref(PCB*);

// implement in process.c
PCB* fetch_pcb(int);
void add_process(PCB*);
int remove_process(PCB *pcb);
size_t pcb_size();
int fetch_all_pcb(PCB **pcbs, int capacity);

static inline void * get_pdir_addr(PCB *p) {
    return (void *)(p->pdir.page_directory_base << 12);
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



#include "init_proc.h"
#endif
