#ifndef __EDF_H__
#define __EDF_H__


void process_pri_add(PCB *p);

PCB *edf();

int edf_priority_queue_empty();

#endif /* __EDF_H__ */

#include "kernel/process.h"

