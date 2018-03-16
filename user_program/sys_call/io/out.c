#include "c_lib.h"

int print_serial(const char *ctl) {
    return syscall(SYS_print_serial, ctl);
}

int puts(const char *ctl) {
    return syscall(SYS_printf, ctl, NULL);
}

int printf(const char *format, ...) {
    void **args = (void **) &format + 1;
    return syscall(SYS_printf, format, args);
}

int prompt() {
    return syscall(SYS_prompt);
}