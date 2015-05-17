#include "kernel/semaphore.h"
#include "kernel/process.h"
#include "kernel/init_proc.h"


#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4

int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
static Sem empty, full, mutex;

void
test_producer(void) {
	while (1) {
		P(&empty);
		P(&mutex);
		if(g % 10000 == 0) {
			printk(".");	// tell us threads are really working
		}
		buf[f ++] = g ++;
		f %= NBUF;
		V(&mutex);
		V(&full);
	}
}

void
test_consumer(void) {
	int get;
	while (1) {
		P(&full);
		P(&mutex);
		get = buf[r ++];
		assert(last == get - 1);	// the products should be strictly increasing
		last = get;
		r %= NBUF;
		V(&mutex);
		V(&empty);
	}
}

void
test_setup(void) {
	create_sem(&full, 0);
	create_sem(&empty, NBUF);
	create_sem(&mutex, 1);
	int i;
	for(i = 0; i < NR_PROD; i ++) {
		add2wake(create_kthread(test_producer));
	}
	for(i = 0; i < NR_CONS; i ++) {
		add2wake(create_kthread(test_consumer));
	}
}