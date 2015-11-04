#include "kernel/syscall.h"

int entry();

void _start2() {
    entry();
    exit();
}