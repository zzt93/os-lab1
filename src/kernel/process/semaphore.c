#include "kernel/semaphore.h"
#include "kernel/process.h"
#include "x86/cpu.h"


void lock() {
    cli();
}

void unlock() {
    sti();
}

static void enqueue(ListHead* l, PCB* p) {
    list_add_after(l, &(p->link));
}

static PCB* dequeue(ListHead* l) {
    list_del(l);
    return list_entry(l, PCB, link);
}

void P(Sem *s) {
    lock();
    NOINTR;
	if(s->token > 0) {
		/* get a token */
        s->token--;
	} else {
		/* go to sleep in the queue */
        // and make the context switched
        sleep_to(&(s->block), enqueue);
        NOINTR;
	}
    unlock();
}

void V(Sem *s) {
    lock();
    NOINTR;
	if(!list_empty(&(s->block))) {
		/* wake up one thread sleeping in the queue */
        wake_up_from(s->block.next, dequeue);
        NOINTR;
	} else {
		/* release the token */
        s->token++;
	}
    unlock();
}


void create_sem(Sem* s, int t) {
    s->token = t;
    list_init(&(s->block));
}