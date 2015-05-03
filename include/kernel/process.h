#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"
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
} PCB;

extern PCB *current;

void add_process(PCB*);
void add2blocked(PCB*);
void sleep();
void wake_up(PCB*);

#endif
