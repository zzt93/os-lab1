#include "kernel/syscall.h"

int puts(const char *ctl) {
	return syscall(SYS_print_serial, ctl);
}
