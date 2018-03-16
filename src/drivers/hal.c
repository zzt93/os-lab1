#include "kernel/kernel.h"
#include "kernel/message.h"
#include "lib/string.h"
#include "drivers/hal.h"

#define NR_DEV 32

static Dev dev_pool[NR_DEV];
static ListHead free, devices;


void init_hal(void) {
    int i = 0;
    list_init(&free);
    for (i = 0; i < NR_DEV; i++) {
        list_add_before(&free, &dev_pool[i].list);
    }
    NOINTR;
    list_init(&devices);
}

static int dev_id_cmp(Dev *d, int id) {
    return d->dev_id == id;
}

static Dev *hal_get(int dev_id) {
    hal_get_(dev_id, dev_id_cmp);
}

/*
static int name_cmp(Dev *d, const char *name) {
    return strcmp(d->name, name) == 0;
}

static Dev *hal_get_byname(const char *name) {
    hal_get_(name, name_cmp);
}

void hal_get_byid(const char *name, pid_t *pid, int dev_id) {
	Dev *dev = hal_get();
	assert(dev != NULL);
	*pid = dev->pid;
	*dev_id = dev->dev_id;
}
*/

static int count_dev = 0;

void hal_register(const char *name, pid_t driver_pid, int *dev_id) {
    lock();
    if (list_empty(&free)) {
        panic("no room for more device");
    }
    /*
    if(hal_get(name) != NULL) {
		panic("device \"%s\" is already registered!\n", name);
	}
    */
    Dev *dev = list_entry(free.next, Dev, list);
    list_del(&dev->list);
    dev->name = name;
    dev->pid = driver_pid;
    // make sure it is unique
    dev->dev_id = *dev_id = count_dev++;
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
        printk("%s pid: #%d, device id: #%d\n", dev->name, dev->pid, dev->dev_id);
    }
    unlock();
}

static size_t
dev_rw(Dev *dev, int type, pid_t reqst_pid, void *buf, off_t offset, size_t len) {
    // why need lock it!!! -- for the lock in asm_do_irq will
    // change the value of $eax, $edx
    //lock();
    //Dev *dev = hal_get(dev_id);
    //unlock();
    //printk("%x ", dev);
    assert(dev != NULL);
    if (dev->dev_id > d_ttyi[NR_TTY - 1] || dev->dev_id < d_ttyi[0]) {
        assert(offset >= 0);
    }
    //printk("%d %s %x ", dev->pid, dev_name, dev);

    /**
       This one shouldn't be static: for may be different devices
       want to read or write at the same time, they may influence
       others' message.
       Using a local variable is safe here for the stack won't
       disappear until we receive a message back.
     */
    Msg m;
    m.src = current->pid;
    m.type = type;
    m.dev_id = dev->dev_id;
    m.req_pid = reqst_pid;
    m.buf = buf;
    m.offset = offset;
    m.len = len;
    // send to that device's driver
    // for tty1 is TTY
    send(dev->pid, &m);
    receive(dev->pid, &m);

    return m.ret;
}

/*
   `reqst_pid` read from device called `aim`,
   `offset` place into `buf`(which has `capacity` bytes )
   offset -- meanless for tty_, for console itself know where
   to read
size_t
dev_read(const char *aim, pid_t reqst_pid, void *buf, off_t offset, size_t capacity) {
	return dev_rw(hal_get_byname(aim), DEV_READ, reqst_pid, buf, offset, capacity);
}

   `reqst_pid` write `len` bytes to device called `aim` from `buf`
   offset -- meanless for tty_, for console itself know where
   to write
size_t
dev_write(const char *aim, pid_t reqst_pid, void *buf, off_t offset, size_t len) {
	return dev_rw(hal_get_byName(aim), DEV_WRITE, reqst_pid, buf, offset, len);
}
 */

/**
   `reqst_pid` read from device dev_id == aim,
   `offset` place into `buf`(which has `capacity` bytes )
   i.e. at most read `capacity` bytes of content
   offset -- meanless for tty_, for console itself know where
   to read
 */
size_t
n_dev_read(int dev_id, pid_t reqst_pid, void *buf, off_t offset, size_t capacity) {
    return dev_rw(hal_get(dev_id), DEV_READ, reqst_pid, buf, offset, capacity);
}

/**
   `reqst_pid` write `len` bytes to device dev_id == `dev_id` from `buf`
   offset -- meanless for tty_, for console itself know where
   to write
 */
size_t
n_dev_write(int dev_id, pid_t reqst_pid, void *buf, off_t offset, size_t len) {
    return dev_rw(hal_get(dev_id), DEV_WRITE, reqst_pid, buf, offset, len);
}
