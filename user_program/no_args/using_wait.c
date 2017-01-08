#include "c_lib.h"
#include "sys_call/io/out.h"

int entry() {
    while(1) {
        puts("wait ... ");
        wait(10);
    }
	return 0;
}
