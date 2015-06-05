#include "kernel/process.h"
#include "kernel/FM.h"
#include "kernel/message.h"

int FM;

/**
   The message sent to FM should specify:
   m->type -- FM_READ
        m->buf
        m->dev_id -- file name
        m->offset
        m->len
        The message send from FM specify:
        m->ret -- len successfully read
 */
static void FM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        switch(m.type) {
            case FM_READ:
                read_file(&m);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void init_FM() {
    PCB* p = create_kthread(FM_job);
    FM = p->pid;
    add2wake(p);
}

