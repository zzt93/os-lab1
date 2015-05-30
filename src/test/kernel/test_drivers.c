#include "drivers/hal.h"
#include "kernel/process.h"
#include "kernel/FM.h"

#define NUM_MBR 512

extern PCB* current;

void read_MBR() {
    char buf[NUM_MBR];
    /**
       TODO why printk will fail
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

void read_FM() {
    static Msg m;
    m.type = FM_READ;
    m.src = current->pid;
    m.dest = FM;
    char buf[NUM_MBR];
    send(FM, &m);
    
    while (true) {
        receive(FM, &m);
        int i;
        for (i = 0; i < NUM_MBR; i++) {
            printk("%c ", buf[i]);
        }
        printk("\n\n");
        send(FM, &m);
    }
}