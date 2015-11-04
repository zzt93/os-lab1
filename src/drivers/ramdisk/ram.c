#include "drivers/ramdisk/ram.h"
#include "assert.h"
#include "lib/string.h"
//#include "kernel/process.h"

extern pid_t RAM_DISK;
const char* ram = "ram";

static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE] = {
    {// user_program/shell.c
    },
    {// user_program/exit.c
    },
};
static uint8_t *disk = (void*)file;

void init_ram() {
    //printk("%c ", disk[0]);
}


/**
   Notice:
   For now, read_ram() can only be used between kernel
   thread, for it use m->buf directly rather than
   using get_pa()
 */
int read_ram(Msg* m) {
    uint32_t i;
    uint8_t data;
    for (i = 0; i < m->len; i++) {
        if (m->offset + i > NR_MAX_FILE * NR_FILE_SIZE - 1) {
            break;
        }
        data = disk[m->offset + i];
        //copy_from_kernel(fetch_pcb(m->req_pid), m->buf + i, &data, 1);
        memcpy(m->buf + i, &data, 1);
    }
    return i;
}