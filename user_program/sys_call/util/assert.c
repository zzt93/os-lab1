//
// Created by zzt on 1/8/17.
//


#include "c_lib.h"

int user_assert(int expr, const char *format, ...) {
    if (!expr) {
        void **args = (void **) &format + 1;
        return syscall(SYS_assert, format, args);
    }
    return 1;
}