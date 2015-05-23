#include "kernel/kernel.h"
#include "kernel/message.h"
#include "lib/string.h"
#include "drivers/hal.h"

#define NR_DEV 16

static Dev dev_pool[NR_DEV];
static ListHead free, devices;


void init_hal(void) {
	int i = 0;
	list_init(&free);
	for (i = 0; i < NR_DEV; i ++) {
		list_add_before(&free, &dev_pool[i].list);
	}
    NOINTR;
	list_init(&devices);
}

static Dev *hal_get(const char *name) {
	lock();
	ListHead *it;
	list_foreach(it, &devices) {
		Dev *dev = list_entry(it, Dev, list);
		assert(dev);
		if (strcmp(dev->name, name) == 0) {
			unlock();
			return dev;
		}
	}
	unlock();
	return NULL;
}

void hal_get_id(const char *name, pid_t *pid, int *dev_id) {
	Dev *dev = hal_get(name);
	assert(dev != NULL);
	*pid = dev->pid;
	*dev_id = dev->dev_id;
}

void hal_register(const char *name, pid_t pid, int dev_id) {
	lock();
	if (list_empty(&free)) {
		panic("no room for more device");
	}
	if(hal_get(name) != NULL) {
		panic("device \"%s\" is already registered!\n", name);
	}
	Dev *dev = list_entry(free.next, Dev, list);
	list_del(&dev->list);
	dev->name = name;
	dev->pid = pid;
	dev->dev_id = dev_id;
	list_add_before(&devices, &dev->list);
	unlock();
}

void hal_list(void) {
	lock();
	ListHead *it;
	printk("listing all registed devices:\n");
	list_foreach(it, &devices) {
		Dev *dev = list_entry(it, Dev, list);
		assert(dev);
		printk("%s #%d, #%d\n", dev->name, dev->pid, dev->dev_id);
	}
	unlock();
}

static size_t
dev_rw(const char *dev_name, int type, pid_t reqst_pid, void *buf, off_t offset, size_t len) {
	Dev *dev = hal_get(dev_name);
	assert(dev != NULL);

	Msg m;
	m.src = current->pid;
	m.type = type;
	m.dev_id = dev->dev_id;
	m.req_pid = reqst_pid;
	m.buf = buf;
	m.offset = offset;
	m.len = len;
	send(dev->pid, &m);
	receive(dev->pid, &m);

	return m.ret;
}

size_t
dev_read(const char *dev_name, pid_t reqst_pid, void *buf, off_t offset, size_t len) {
	return dev_rw(dev_name, DEV_READ, reqst_pid, buf, offset, len);
}

size_t
dev_write(const char *dev_name, pid_t reqst_pid, void *buf, off_t offset, size_t len) {
	return dev_rw(dev_name, DEV_WRITE, reqst_pid, buf, offset, len);
}
