#include "drivers/hal.h"
#include "kernel/process.h"
#include "kernel/manager/FM.h"

#define NUM_MBR 512

extern PCB *current;

void read_MBR() {
    char buf[NUM_MBR];
    /**
       why printk will fail when switch then interrupt
       for I use lock() in context switch which change the
       values of $eax, $edx
     */
    int offset = 0;
    while (true) {
        n_dev_read(d_ide, current->pid, buf, offset, NUM_MBR - offset);
        int i;
        for (i = 0; i < NUM_MBR - offset; i++) {
            printk("%x ", buf[i]);
        }
        printk("\n\n");
    }
}

static void set_mes(Msg *m, char *buf) {
    m->type = FM_ram_read;
    m->src = current->pid;
    m->dest = FM;
    m->buf = buf;
    m->offset = 0;
    m->len = 500;
}

void read_FM() {
    static Msg m;
    char buf[NUM_MBR];
    set_mes(&m, buf);
    send(FM, &m);

    while (true) {
        receive(FM, &m);
        int i;
        for (i = 0; i < m.len; i++) {
            printk("%x ", buf[i]);
        }
        printk("\n\n");
        set_mes(&m, buf);
        send(FM, &m);
    }
}