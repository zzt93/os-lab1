#include "kernel/syscall.h"
#include "sys_call/io/io.h"
#include "lib/string.h"


#define BUF_SZ 256


int entry() {
    char cmd[BUF_SZ], copy[BUF_SZ];
    char *save[10] = {0};
    int filename = -1;
    int pid, count, res;
    while(1) {
		prompt();
        memset(cmd, 0, BUF_SZ);
        read_line(cmd, BUF_SZ);
		memcpy(copy, cmd, BUF_SZ);
        count = split(copy, ' ', save);
		if (count <= 1) {
			printf("Unknown command: %s", cmd);
			continue;
		}
        filename = to_int(save[0]);
		// TODO check file existence and whether it is executable -- many be checked by exec
        if((pid = fork()) == 0) {
            res = exec(filename, save[1] - copy + cmd);
			if (res == 0) {
				printf("Unknown command or wrong args: %s", cmd);
				continue;
			}	
        }
        else {
            waitpid(pid);
        }
    }
    return 0;
}
