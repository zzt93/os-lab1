#include "drivers/hal.h"
#include "drivers/tty/tty.h"
#include "drivers/tty/tty4_p.h"

#include "kernel/process.h"

char * user_name = "zzt@os_lab: ";


static char buf[BUF_SIZE];
static int len = 0;


void terminal_job() {

    while(1) {
    }
}

void tty4_printc(char c) {
    NOINTR;
    buf[len++] = c;
    if (c == NEW_LINE) {
        flush();
    }
}

void flush() {
    if (len == 0) {
        return;
    }
    NOINTR;
    n_dev_write(d_ttyi[NOW_TERMINAL], current->pid, buf, 0, len);
    len = 0;
}