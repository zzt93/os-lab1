#include "kernel/syscall.h"
#include "sys_call/io/out.h"

int main() {
    while(1) {
        puts("wait ... ");
        wait(10);
    }
	return 0;
}