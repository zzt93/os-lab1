#include "c_lib.h"

int entry(char *);

void _start(char *args) {
    entry(args);
    exit();
}

