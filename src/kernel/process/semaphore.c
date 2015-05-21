#include "kernel/semaphore.h"
#include "kernel/process.h"
#include "x86/cpu.h"

extern PCB* current;

void lock() {
    cli();
    current->count_of_lock++;
}

void unlock() {
    NOINTR;
    assert(current != NULL);
    current->count_of_lock--;
    assert(current->count_of_lock >= 0);
    //printk("#%d count is %d ", current->pid,current->count_of_lock);
    if (current->count_of_lock == 0) {
        sti();
    }
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