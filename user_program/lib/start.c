#include "kernel/syscall.h"

int entry(char *);

void _start(char *args) {
    entry(args);
    exit();
}