#include "sys_call/io/out.h"
#include "kernel/syscall.h"

int entry(char *args) {
    printf("\n\nargs is %s\n\n", args);
    return 1;
}