#include "kernel/syscall.h"

void read_line(char *str, int capacity) {
    syscall(SYS_read_line, str, capacity);
}