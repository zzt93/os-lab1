#include "adt/heap.h"
#include "kernel/message.h"
#include "kernel/process.h"
#include "drivers/time.h"

extern PCB* current;

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

HEAP(Timer, 10, cmp)

int kwait(Msg *m) {
    Timer t;
    set_timer(&t, m->i[0], m->i[1]);
    add(t);
    return 1;
}

void update_timer() {
    // check if any timer is finished
    if (empty()) {
        return;
    }
    Timer t;
    Msg m;
    m.src = current->pid;
    while ((t = max()).time == 1) {
        pop_max();
        send(t.pid, &m);
    }
    int i = 0;
    Timer *tmp;
    heap_each(i, tmp) {
        tmp->time --;
    }
}