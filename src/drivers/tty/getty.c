#include "kernel/kernel.h"
#include "lib/string.h"
#include "kernel/init_proc.h"
#include "drivers/tty/tty.h"
#include "drivers/hal.h"

static int tty_idx = 1;

static void
getty(void) {
    int capacity = 256;
	char name[] = "tty0", buf[capacity];
	lock();
	name[3] += (tty_idx ++);
	unlock();

    int len = 0;
	while(1) {
		/* Insert code here to do these:
		 * 1. read key input from ttyd to buf (use dev_read())
		 * 2. convert all small letters in buf into capitcal letters
		 * 3. write the result on screen (use dev_write())
        */
        len = dev_read(name, current->pid, buf, 0, capacity);
        int i;
        for (i = 0; i < len; i++) {
            buf[i] = to_upper(buf[i]);
        }
        dev_write(name, current->pid, buf, 0, len);
	}
}

void
init_getty(void) {
	int i;
	for(i = 0; i < NR_TTY - 1; i ++) {
		add2wake(create_kthread(getty));
	}
}


