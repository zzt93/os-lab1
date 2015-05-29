#include "kernel/message.h"
#include "drivers/hal.h"

void read_file(Msg* m) {
    do_read();
}

static void do_read(int file_name, uint8_t *buf, off_t offset, size_t len) {
    
}