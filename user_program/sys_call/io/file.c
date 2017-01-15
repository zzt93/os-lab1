#include "c_lib.h"


int open(const char *name) {
    return syscall(SYS_open, name);
}

int createfile(const char *name) {
    return syscall(SYS_create, name);
}

int makedir(const char *dir) {
    return syscall(SYS_make, dir);
}

int delfile(const char *name) {
    return syscall(SYS_del, name);
}

int deldir(const char *dir) {
    return syscall(SYS_del, dir);
}

int listdir(const char *dir, char *buf, int len) {
    return syscall(SYS_lsdir, dir, buf, len);
}

int chdir(const char *dir) {
    return syscall(SYS_chdir, dir);
}

int print_cwd_path() {
    return syscall(SYS_pwd);
}