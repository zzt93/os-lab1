#include "kernel/kernel.h"
#include "lib/string.h"
#include "kernel/init_proc.h"
#include "drivers/tty/tty.h"
#include "drivers/hal.h"

static int tty_idx = 0;

static void
getty(void) {
    int capacity = 256;
    char buf[capacity];
    int in = 0;
    lock();
    in = (tty_idx++);
    unlock();

    int len = 0;
    while (1) {
        /* Insert code here to do these:
         * 1. read key input from ttyd to buf (use dev_read())
         * 2. convert all small letters in buf into capitcal letters
         * 3. write the result on screen (use dev_write())
        */
        len = n_dev_read(d_ttyi[in], current->pid, buf, 0, capacity);
        int i;
        for (i = 0; i < len; i++) {
            buf[i] = to_upper(buf[i]);
        }
        n_dev_write(d_ttyi[in], current->pid, buf, 0, len);
    }
}

void
init_getty(void) {
    int i;
    for (i = 0; i < NR_TTY - 1; i++) {
        add2wake(create_kthread(getty));
    }
}


