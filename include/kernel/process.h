#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"
#include "kernel/semaphore.h"

#define KSTACK_SIZE 4096
#define PCB_SIZE (sizeof (PCB))

typedef enum {
    IDLE, SLEEPED, WAKED,
} PROCESS_STATE;
/*
  for the stack is grow to small address,
  the first item on the stack, should be the
  last in the struct

  在asm_do_irq中, 代码将会把A的通用寄存器保存到A的堆栈上,
  这些寄存器的内容连同之前保存的#irq和
  硬件保存的EFLAGS, CS, EIP形成了"陷阱帧"(trap frame)的数据结构
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
    uint16_t gs, fs, ed, ds;
    int irq;
    int error_code;
    unsigned int eip;
    uint16_t cs;
    unsigned int eflags;
    */
    // stack of user
    uint8_t kstack[KSTACK_SIZE];
    // pid
    int pid;
    PROCESS_STATE state;
    ListHead link;
    ListHead mes;
    Sem mes_lock;
} PCB;

extern PCB *current;

// implement in schedule.c
void add2wake(PCB*);
void add2sleeped(PCB*);
void sleep();
void sleep_to(ListHead*, void (*)(ListHead*, PCB*));
void wake_up_from(ListHead*, PCB* (*)(ListHead*));
void wake_up(PCB*);

// implement in process.c
PCB* fetch_pcb(int);
void add_process(PCB*);

#define IDLE_ID 0
#define START_ID (IDLE_ID+1)

#define ANY -1

#endif
