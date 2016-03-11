
#include "drivers/hal.h"
#include "kernel/manager/FM.h"


size_t read_char_dev_file(int dev_id, char *buf, int len) {
    int read = n_dev_read(dev_id, FM, buf, -1, len);
    assert(len == read);
    return read;
}

size_t write_char_dev_file(int dev_id, char *buf, int len) {
    size_t write = n_dev_write(dev_id, FM, buf, -1, len);
    return write;
}
