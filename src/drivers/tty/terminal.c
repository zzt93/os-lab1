#include "drivers/hal.h"
#include "drivers/tty/tty.h"
#include "drivers/tty/tty4_p.h"

#include "kernel/process.h"

static char name[] = "tty4";

static char buf[BUF_SIZE];
static int len = 0;


void terminal() {

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
    dev_write(name, current->pid, buf, 0, len);
    len = 0;
}