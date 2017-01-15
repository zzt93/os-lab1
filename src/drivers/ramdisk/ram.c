#include "drivers/ramdisk/ram.h"
#include "assert.h"
#include "lib/string.h"
//#include "kernel/process.h"

extern pid_t RAM_DISK;
const char* ram = "ram";

static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE] = {
    {
            /*user_program/no_args/shell.ar*/
    },
    {
            /*user_program/args/ls.ar*/
    },
    {
            /*user_program/args/exit.ar*/
    },
    {
            /*user_program/args/echo.ar*/
    },
};
static uint8_t *disk = (void*)file;

//static int file_size[NR_MAX_FILE];

void init_ram() {
//    int i;
//    for (i = 0; i < NR_MAX_FILE; i++) {
//        file_size[i] = sizeof(file[i]) / sizeof(char);
//    }

    //printk("%c ", disk[0]);
}


/**
  * <h3>Notice:</h3>
  * <p>For now, read_ram() can only be used between kernel thread,
  * for it use m->buf directly rather than using get_pa()</p>
  * <p>Ram is the old memory file system, now only works as the media
  * to copy compiled executable from outer linux. So, it is transparent for
  * user process.</p>
  *
  * @param m
  * @return
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
    m->ret = SUCC;
    return i;
}

const uint8_t *const get_a_ram_file(int i, int *m_size) {
    assert(i >= 0 && i < NR_MAX_FILE);
    *m_size = NR_FILE_SIZE;
    return file[i];
}
