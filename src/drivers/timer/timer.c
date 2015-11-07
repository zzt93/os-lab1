#include "kernel/kernel.h"
#include "x86/x86.h"
#include "drivers/hal.h"
#include "drivers/time.h"
#include "lib/string.h"
#include "kernel/message.h"
#include "kernel/init_proc.h"

#define PORT_TIME 0x40
#define PORT_RTC  0x70
#define FREQ_8253 1193182

pid_t TIMER;
int d_timer;

static long jiffy = 0;
static Time rt;
const char* timer = "timer";

static void update_jiffy(void);
static void init_i8253(void);
static void init_rt(void);
static void timer_driver_thread(void);

/**
   设置时钟中断的频率(100Hz)
   初始化系统时间(系统时间将会从2000/01/01 00:00:00开始, 但不会影响系统的正常运行)
   注册顶半处理update_jiffy(), 每次时钟中断到来时会更新变量jiffy的值, 这个变量记录了距离系统初始化结束期间时钟中断到来的次数
   创建TIMER主线程
   注册设备"timer"
 */
void init_timer(void) {
	init_i8253();
	init_rt();
	add_irq_handle(0, update_jiffy);
	PCB *p = create_kthread(timer_driver_thread);
	TIMER = p->pid;
	add2wake(p);
	hal_register(timer, TIMER, &d_timer);
}


static void
timer_driver_thread(void) {
	static Msg m;
    static int dest = -1;
	while (true) {
		receive(ANY, &m);
        dest = m.src;
		switch (m.type) {
            case NEW_TIMER:
                kwait(&m);
                continue;
			default: assert(0);
		}
        m.src = current->pid;
        send(dest, &m);
	}
}

long
get_jiffy() {
	return jiffy;
}

static int
md(int year, int month) {
	bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
	static int tab[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	return tab[month] + (leap && month == 2);
}

// @see edf.c
void to_ddl_each_update();

static void
update_jiffy(void) {
	jiffy ++;

    update_non_block_timer();
	if (jiffy % HZ == 0) {
        // TODO change it to more accurate -- i.e. update_timer() not every 100 times,
        // less the count, more accurate the timer
        /*
          if (jiffy % 10 == 0) {
          update_timer();
          }
         */
        update_timer();
        to_ddl_each_update();
		rt.second ++;
		if (rt.second >= 60) { rt.second = 0; rt.minute ++; }
		if (rt.minute >= 60) { rt.minute = 0; rt.hour ++; }
		if (rt.hour >= 24)   { rt.hour = 0;   rt.day ++;}
		if (rt.day >= md(rt.year, rt.month)) { rt.day = 1; rt.month ++; }
		if (rt.month >= 13)  { rt.month = 1;  rt.year ++; }
	}
}

static void
init_i8253(void) {
	int count = FREQ_8253 / HZ;
	assert(count < 65536);
	out_byte(PORT_TIME + 3, 0x34);
	out_byte(PORT_TIME, count & 0xff);
	out_byte(PORT_TIME, count >> 8);
}

static void
init_rt(void) {
	memset(&rt, 0, sizeof(Time));
	/* Optional: Insert code here to initialize current time correctly */

}

/**
   get time by counting times of timer interrupt
   100Hz, i.e. 1s = 100 interrupts
 */
void
get_time(Time *tm) {
	memcpy(tm, &rt, sizeof(Time));
}
