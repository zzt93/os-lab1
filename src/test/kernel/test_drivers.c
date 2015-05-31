#include "drivers/hal.h"
#include "kernel/process.h"
#include "kernel/FM.h"

#define NUM_MBR 512

extern PCB* current;

void read_MBR() {
    char buf[NUM_MBR];
    /**
       TODO why printk will fail when switch then interrupt
     */
    int offset = 510;
    while (true) {
        dev_read(hda, current->pid, buf, offset, NUM_MBR - offset);
        int i;
        for (i = 0; i < NUM_MBR - offset; i++) {
            printk("%x ", buf[i]);
        }
        printk("\n\n");
    }
}

static void set_mes(Msg* m, char *buf) {
    m->type = FM_READ;
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
            printk("%d ", buf[i]);
        }
        printk("\n\n");
        set_mes(&m, buf);
        send(FM, &m);
    }
}