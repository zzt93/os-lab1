#include "kernel/semaphore.h"
#include "kernel/trapFrame.h"


#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4

int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
Sem empty, full, mutex;

void
test_producer(void) {
	while (1) {
		P(&mutex);
		P(&empty);
		if(g % 10000 == 0) {
			printk(".");	// tell us threads are really working
		}
		buf[f ++] = g ++;
		f %= NBUF;
		V(&full);
		V(&mutex);
	}
}

void
test_consumer(void) {
	int get;
	while (1) {
		P(&mutex);
		P(&full);
		get = buf[r ++];
		assert(last == get - 1);	// the products should be strictly increasing
		last = get;
		r %= NBUF;
		V(&empty);
		V(&mutex);
	}
}

void
test_setup(void) {
	create_sem(&full, 0);
	create_sem(&empty, NBUF);
	create_sem(&mutex, 1);
	int i;
	for(i = 0; i < NR_PROD; i ++) {
		wakeup(create_kthread(test_producer));
	}
	for(i = 0; i < NR_CONS; i ++) {
		wakeup(create_kthread(test_consumer));
	}
}