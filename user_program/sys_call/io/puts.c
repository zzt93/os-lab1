#include "kernel/syscall.h"

int puts(const char *ctl, ...) {
	return syscall();
}
