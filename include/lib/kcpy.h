#ifndef __KCPY_H__
#define __KCPY_H__

#include "kernel/process.h"

void copy_from_kernel(PCB *pcb, void *dest, void *src, int len);

void copy_to_kernel(PCB *pcb, void *dest, void *src, int len);

void strcpy_to_kernel(PCB *pcb, char *dest, char *src);

void strcpy_from_kernel(PCB *pcb, char *dest, char *src);

#endif /* __KCPY_H__ */
