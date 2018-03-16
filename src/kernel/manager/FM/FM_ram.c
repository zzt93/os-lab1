#include "kernel/message.h"
#include "kernel/process.h"
#include "drivers/hal.h"
#include "drivers/ramdisk/ram.h"
#include "assert.h"

extern PCB *current;

static int do_read(uint8_t *buf, off_t offset, size_t len) {
    assert(buf > 0);
    assert(offset >= 0);
    assert(len >= 0);
    return n_dev_read(d_ramdisk, current->pid, buf, offset, len);
    //printk("buf is: %s", buf);
}

static int file_check(Msg *m) {
    // existence check
    if (m->dev_id >= NR_MAX_FILE
        || m->offset >= NR_FILE_SIZE) {
        return FAIL;
    }
    // file type check
    return SUCC;
}

size_t ram_read_file(Msg *m) {
    size_t res = 0;
    if (file_check(m)) {
        res = do_read(m->buf, m->offset + m->dev_id * NR_FILE_SIZE, m->len);
        m->ret = SUCC;
    } else {
        m->ret = FAIL;
    }
    return res;
}

