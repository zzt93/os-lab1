#include "c_lib.h"


int entry() {
    while(1) {
        int filename = 1;
        char *cmd = "gcc -o2 test.c";
        int pid;
        if( (pid = fork()) == 0) {
            ram_exec(filename, cmd);
            exit();
        } else {
            waitpid(pid);
        }
    }
	return 0;
}
