#include "drivers/hal.h"
#include "kernel/process.h"

#define NUM_MBR 512

extern PCB* current;

void read_MBR() {
    char buf[NUM_MBR];
    int offset = 0;
    while (true) {
        dev_read(hda, current->pid, buf, offset, NUM_MBR - offset);
        int i;
        for (i = 0; i < NUM_MBR - offset; i++) {
            printk("%x ", buf[i]);
        }
        printk("\n\n");
    }
}