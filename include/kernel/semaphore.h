#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "adt/list.h"

typedef struct Semaphore {
	int token;
	ListHead block;		/* blocking queue */
}Sem;

void P(Sem *);
void V(Sem *);
void lock();
void unlock();


void create_sem(Sem*, int);

#endif /* __SEMAPHORE_H__ */