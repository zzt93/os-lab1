#include "drivers/ramdisk/ram.h"
#include "assert.h"
#include "lib/kcpy.h"
//#include "kernel/process.h"

extern pid_t RAM_DISK;
const char* ram = "ram";

static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE] = {
    {'h', 'e', 'l', 'l', 'o'},
    {'w', 'o'},
};
static uint8_t *disk = (void*)file;

void init_ram() {
    printk("%c ", disk[0]);
}


void read_ram(Msg* m) {
    uint32_t i;
    uint8_t data;
    for (i = 0; i < m->len; i++) {
        if (m->offset + i > NR_MAX_FILE * NR_FILE_SIZE - 1) {
            break;
        }
        data = disk[m->offset + i];
        copy_from_kernel(
            fetch_pcb(m->req_pid),
            m->buf + i,
            &data,
            1);
    }
    m->ret = i;
    pid_t dest = m->src;
    m->src = RAM_DISK;
    send(dest, m);
}