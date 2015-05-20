#include "kernel/kernel.h"
#include "kernel/init_proc.h"
#include "drivers/tty/tty.h"
#include "drivers/hal.h"

static int tty_idx = 1;

static void
getty(void) {
	char name[] = "tty0";//, buf[256];
	lock();
	name[3] += (tty_idx ++);
	unlock();

	while(1) {
		/* Insert code here to do these:
		 * 1. read key input from ttyd to buf (use dev_read())
		 * 2. convert all small letters in buf into capitcal letters
		 * 3. write the result on screen (use dev_write())
		 */
        //dev_read(name, current->pid, buf, 0, 1);
	}
}

void
init_getty(void) {
	int i;
	for(i = 0; i < NR_TTY; i ++) {
		add2wake(create_kthread(getty));
	}
}


