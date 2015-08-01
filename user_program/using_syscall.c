#include "kernel/syscall.h"


int entry() {
    while(1) {
        int filename = 2;
        char *cmd = "gcc -o2 test.c";
        int pid;
        if( (pid = fork()) == 0) {
            exec(filename, cmd);
            exit();
        } else {
            waitpid(pid);
        }
    }
	return 0;
}
