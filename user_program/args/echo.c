#include "sys_call/io/out.h"
#include "c_lib.h"


int entry(char *args) {
    printf("\n\nargs is %s\n\n", args);
    return 1;
}