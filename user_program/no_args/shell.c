#include "kernel/syscall.h"
#include "sys_call/io/io.h"
#include "libu/string.h"

#define BUF_SZ 512


int entry() {
    char cmd[BUF_SZ];
    char *save[10];
    int filename = -1;
    int pid;
    while(1) {
        read_line(cmd);
        split(cmd, ' ', save);
        filename = to_int(save[0]);
        if((pid = fork()) == 0) {
            exec(filename, cmd);
        }
        else {
            waitpid(pid);
        }
        return 0;
    }
}
