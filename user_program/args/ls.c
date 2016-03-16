#include "sys_call/io/out.h"
#include "kernel/manager/f_dir.h"
#include "kernel/syscall.h"
#include "error.h"

#define LS_NAME_MAX_LEN 512

int entry(char *args) {
    char buf[LS_NAME_MAX_LEN];
    int res = listdir(args, buf, LS_NAME_MAX_LEN);
    if (!has_error(res)) {
        Dir_entry *entry = (Dir_entry*)buf;
        int i;
        for (i = 0; i < res; i++) {
            printf("%s, ", entry[i].filename);
        }
    } else {
        printf("%s: %s", get_err_msg(res), args);
    }
    return res;
}