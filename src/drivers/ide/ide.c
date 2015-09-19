#include "kernel/kernel.h"
#include "kernel/init_proc.h"
#include "kernel/message.h"
#include "kernel/process.h"
#include "drivers/hal.h"
#include "ide.h"
#include "drivers/time.h"
#include "lib/string.h"

#include "kernel/manager/block.h"

#define WRITEBACK_TIME  1  /* writeback cache for every 1 second */

pid_t IDE;
const char* hda = "hda";
int d_ide;

static void ide_intr(void);
static void time_intr(void);
static void ide_driver_thread(void);

void cache_init(void);
void cache_writeback(void);
uint8_t read_byte(uint32_t);
void write_byte(uint32_t, uint8_t);

/**
   初始化IDE的cache
   注册顶半处理ide_intr()和time_intr(), 前者负责在磁盘就绪时通知IDE, 后者负责通知IDE是否到了需要回写cache的时间
   创建IDE主线程
   注册设备"hda"
*/
void
init_ide(void) {
	cache_init();
	add_irq_handle(14, ide_intr);
	add_irq_handle(0 , time_intr);
	PCB *p = create_kthread(ide_driver_thread);
	IDE = p->pid;
	hal_register(hda, IDE, &d_ide);
	add2wake(p);
}

static inline
void disk_msg_check(Msg *m) {
    assert(m->offset >= super_start
        && m->offset < block_start + block_area_size);
    assert(m->len < block_area_size);
}

static void
ide_driver_thread(void) {
	static Msg m;

	while (true) {
		receive(ANY, &m);

		if (m.src == MSG_HARD_INTR) {
            printk("msg hard interrupt ");
			if (m.type == IDE_WRITEBACK) {
				cache_writeback();
			} else {
				panic("IDE interrupt is leaking");
			}
		} else {
            //if (m.src != IDLE_ID) {// mean it is the init of file system
            if (block_area_size != 0 && block_start != 0) {
                disk_msg_check(&m);
            }
            if (m.type == DEV_READ) {
                printk("device read ");
                uint32_t i;
                uint8_t data;
                printk("device read %d ", m.len);
                for (i = 0; i < m.len; i ++) {
                    data = read_byte(m.offset + i);
                    //copy_from_kernel(fetch_pcb(m.req_pid), m.buf + i, &data, 1);
                    memcpy(m.buf + i, &data, sizeof data);
                }
                printk("device read ");
                m.ret = i;
                m.dest = m.src;
                m.src = IDE;
                printk("device read ");
                send(m.dest, &m);
            } else if (m.type == DEV_WRITE) {
                printk("device write ");
                uint32_t i;
                uint8_t data;
                for (i = 0; i < m.len; i ++) {
                    memcpy(&data, m.buf + i, sizeof data);
                    //copy_to_kernel(fetch_pcb(m.req_pid), &data, m.buf + i, 1);
                    write_byte(m.offset + i, data);
                }
                m.ret = i;
                m.dest = m.src;
                m.src = IDE;
                send(m.dest, &m);
            } else {
                assert(0);
            }
        }
	}
}

static void
ide_intr(void) {
    // for it is NOINTR, so using static message is fine
    NOINTR;
	static Msg m;
	m.type = IDE_READY;
	m.src = MSG_HARD_INTR;
	send(IDE, &m);
}

static void
time_intr(void) {
    NOINTR;
	static Msg m;
	static uint32_t counter = 0;
	counter = (counter + 1) % (WRITEBACK_TIME * HZ);
	if (counter == 0) {
		m.type = IDE_WRITEBACK;
		m.src = MSG_HARD_INTR;
		send(IDE, &m);
	}
}

