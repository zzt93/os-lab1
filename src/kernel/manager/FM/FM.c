#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/message.h"

#include "kernel/manager/fd.h"

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
        pid_t dest = m.src;
        switch(m.type) {
            case FM_read:
                read_file(&m);
                break;
            case FM_write:
                assert(false);
                break;
            case FM_open:
                m.ret = open_file(&m);
                break;
            case FM_close:
                m.ret = close_file(&m);
                break;
            case FM_dup:
                m.ret = dup_file(&m);
                break;
            case FM_dup2:
                m.ret = dup2_file(&m);
                break;
            case FM_lseek:
                m.ret = lseek_file(&m);
                break;
            default:
                assert(false);
                break;
        }
        m.src = FM;
        send(dest, &m);
    }
}

void init_FM() {
    PCB* p = create_kthread(FM_job);
    FM = p->pid;
    add2wake(p);
}

