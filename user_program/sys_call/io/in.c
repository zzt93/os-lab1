#include "kernel/syscall.h"

int read_line(char *str, int capacity) {
    return syscall(SYS_read_line, str, capacity);
}
