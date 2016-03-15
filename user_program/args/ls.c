#include "sys_call/io/out.h"
#include "kernel/syscall.h"

#define LS_NAME_MAX_LEN 128

int entry(char *args) {
    char buf[LS_NAME_MAX_LEN];
    int res = listdir(args, buf);
    printf("%s\n", buf);
    return res;
}