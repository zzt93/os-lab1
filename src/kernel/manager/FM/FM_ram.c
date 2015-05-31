#include "kernel/message.h"
#include "kernel/process.h"
#include "drivers/hal.h"

extern PCB* current;

static void do_read(uint8_t *buf, off_t offset, size_t len) {
    dev_read(ram, current->pid, buf, offset, len);
    //printk("buf is: %s", buf);
}

void read_file(Msg* m) {
    do_read(m->buf, m->offset, m->len);
    m->ret = m->len;
    pid_t dest = m->src;
    m->src = current->pid;
    send(dest, m);
}

