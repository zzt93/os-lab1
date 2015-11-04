#include "kernel/syscall.h"

int puts(const char *ctl) {
	return syscall(SYS_print_serial, ctl);
}

int printf(const char *format, ...) {
    void **args = (void **)&format + 1;
    return syscall(SYS_printf, format, args);
}

int prompt() {
    return syscall(SYS_prompt);
}