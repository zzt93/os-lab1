#include "kernel/kernel.h"
#include "kernel/message.h"
#include "drivers/hal.h"
#include "drivers/tty/tty.h"
#include "lib/kcpy.h"

pid_t TTY;

void send_keymsg(void);

void init_console(void);
void init_getty(void);
static void ttyd(void);

void init_tty(void) {
    // register key-press handler
	add_irq_handle(1, send_keymsg);
	PCB *p = create_kthread(ttyd);
	TTY = p->pid;
	add2wake(p);
	init_console();
	init_getty();
}

static void
ttyd(void) {
	static Msg m;

	while (1) {
        INTR;
		receive(ANY, &m);
        //printk("position1\n");
        INTR;
		if (m.src == MSG_HARD_INTR) {
			switch (m.type) {
				case MSG_TTY_GETKEY:
                    INTR;
                    //printk("position2\n");
					readkey();
                    INTR;
					break;
				case MSG_TTY_UPDATE:
                    //printk("position3\n");
					update_banner();
					break;
				default: assert(0);
			}
		} else {
            INTR;
			switch(m.type) {
				case DEV_READ:
					read_request(&m);
					break;
				case DEV_WRITE:
					if (m.dev_id >= 0 && m.dev_id < NR_TTY) {
						char c;
						int i;
						for (i = 0; i < m.len; i ++) {
							copy_to_kernel(fetch_pcb(m.req_pid), &c, (char*)m.buf + i, 1);
							consl_writec(&ttys[m.dev_id], c);
						}
						consl_sync(&ttys[m.dev_id]);
					}
					else {
						assert(0);
					}
					m.ret = m.len;
					pid_t dest = m.src;
					m.src = current->pid;
					send(dest, &m);
					break;
				default:
                    printk("type %d", m.type);
                    assert(0);
			}
		}
	}
}

