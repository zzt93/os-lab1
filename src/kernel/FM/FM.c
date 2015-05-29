#include "kernel/process.h"
#include "kernel/FM.h"
#include "kernel/message.h"

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
    add2wake(p);
}

