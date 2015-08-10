#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/message.h"

int FM;

/**
   The message sent to FM should specify:
   m->type -- FM_read
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
        pid_t dest = m->src;
        switch(m.type) {
            case FM_read:
                m.ret = read_file(&m);
                break;
            case FM_write:
                assert(false);
                break;
            case FM_open:
                m.ret = open_file(&m);
                break;
            default:
                assert(false);
                break;
        }
        m->src = RAM_DISK;
        send(dest, m);
    }
}

void init_FM() {
    PCB* p = create_kthread(FM_job);
    FM = p->pid;
    add2wake(p);
}

