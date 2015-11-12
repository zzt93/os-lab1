#include "adt/heap.h"
#include "kernel/message.h"
#include "kernel/process.h"
#include "drivers/time.h"

#include "lib/malloc.h"

/**
   Notice:
   if you send timer a message, but not using receive to
   wait for it(i.e. you want your process to block to wait
   some time), it can still work for timer will send a
   reply message and put it in the message box for your process
   to receive later, but time is not accurate any more.
   So had better receive() before your time run out
 */
extern PCB* current;

static
void set_timer(Timer *t, int time, int pid) {
    t->time = time;
    t->pid = pid;
}

static int cmp(Timer *t1, Timer *t2) {
    if (t1->time < t2->time) {
        return 1;
    } else if (t1->time > t2->time) {
        return -1;
    }
    return 0;
}

HEAP(Timer*, 10, cmp, block_timer)

int kwait(Msg *m) {
    Timer *t = kmalloc(sizeof(Timer));
    if (t == NULL) {
        return 0;
    }
    set_timer(t, m->i[0], m->i[1]);
    block_timer_add(t);
    return 1;
}

void update_timer() {
    // check if any timer is finished
    if (block_timer_empty()) {
        return;
    }
    Timer *t;
    Msg m;
    // using current->pid is wrong, for current is not fixed
    m.src = TIMER;
    while (!block_timer_empty() && (t = block_timer_max())->time == 1) {
        block_timer_pop_max();
        send(t->pid, &m);
        kfree(t);
    }
    int i = 0;
    Timer *tmp;
    heap_each(i, tmp) {
        tmp->time --;
    }
}

