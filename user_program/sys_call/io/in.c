#include "kernel/syscall.h"

void read_line(char *str) {
    syscall(SYS_read_line, str);
}