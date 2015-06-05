#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/MM.h"

int MM;

/**
   The message sent to MM should specify:
   m->type == NEW_PAGE:
      m->buf -- the address of page directory
      m->offset -- allocate pages from this offset
      m->len -- allocate no less than this len

      The message send from MM specify:
      m->buf -- the allocated pages' starting physical address
 */
static void MM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        switch(m.type) {
            //case NEW_PDIR:
            case NEW_PAGE:
                init_va(&m);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void init_MM() {
    PCB* p = create_kthread(MM_job);
    MM = p->pid;
    add2wake(p);
}