#include "kernel/syscall.h"

int createfile(const char *name) {
    return syscall(SYS_create, name);
}

int makedir(const char *name) {
    return syscall(SYS_make, name);
}

int delfile(const char *name) {
    return syscall(SYS_del, name);
}

int deldir(const char *name) {
    return syscall(SYS_del, name);
}

int listdir(const char *name, char *buf) {
    return syscall(SYS_lsdir, name, buf);
}

int chdir(const char *name) {
    return syscall(SYS_chdir, name);
}

int print_cwd_path() {
    return syscall(SYS_pwd);
}