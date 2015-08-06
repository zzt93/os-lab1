#include "kernel/syscall.h"

int entry();

void __start() {
    entry();
    exit();
}