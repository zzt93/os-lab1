#ifndef __PROCESS_H__
#define __PROCESS_H__

typedef struct PCB {
	void *tf;
} PCB;

extern PCB *current;

#endif
