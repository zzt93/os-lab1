#include "kernel/syscall.h"

int createfile(const char *name) {
    return syscall(SYS_createfile, name);
}

int makedir(const char *name) {
    return syscall(SYS_makedir, name);
}

int delfile(const char *name) {
    return syscall(SYS_delfile, name);
}

int deldir(const char *name) {
    return syscall(SYS_deldir, name);
}

int listdir(const char *name, char *buf) {
    return syscall(SYS_listdir, name, buf);
}