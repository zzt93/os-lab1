#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/MM.h"

int MM;

/**
   The message sent to MM should specify:
   m->type == NEW_PAGE:
      m->i[0] -- not used
      m->i[1] -- the flag of page entry
      m->buf -- the address of page directory
      m->offset -- allocate pages from this offset
      m->len -- allocate no less than this len

      @return: The message send from MM specify:
      m->buf -- the allocated pages' starting physical address

   m->type == COPY_page:
      m->i[0] -- src PCB's address
      m->i[1] -- dest PCB's address
      @return:
 */
static void MM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);

        pid_t dest = m.src;
        switch(m.type) {
            //case NEW_PDIR:
            case NEW_PAGE:
                m.ret = init_va(&m);
                break;
            case COPY_page:
                m.ret = page_copy(&m);
                break;
            case FREE_page:
                m.ret = page_free(&m);
                break;
            default:
                assert(false);
                break;
        }
        m.src = current->pid;
        send(dest, &m);
    }
}

void init_MM() {
    PCB* p = create_kthread(MM_job);
    MM = p->pid;
    add2wake(p);
}