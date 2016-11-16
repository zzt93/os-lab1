#include "kernel/syscall.h"
#include "thread_annotations.h"
#include "shell.h"

static char simple_path[BUF_SZ];
/**
 * Rules:
 * <li>remove redundant "/"</li>
 * <li>remove "." ".."</li>
 *
 * @param path
 * @return simplified path accroding to rules
 */
const char *simplify_path(const char *path)
NOT_THREAD_SAFE("assume only one thread invoke it") {
    size_t len = 0;
    if (*path == '/') { // absolute path

    } else {

    }
    int slash_i = 0;
    while (*path != '\0') {
        if (*path == '/') {

        }
        path++;
    }
    simple_path[len] = '\0';
    return simple_path;
}


int open(const char *name) {
    return syscall(SYS_open, simplify_path(name));
}

int createfile(const char *name) {
    return syscall(SYS_create, simplify_path(name));
}

int makedir(const char *dir) {
    return syscall(SYS_make, simplify_path(dir));
}

int delfile(const char *name) {
    return syscall(SYS_del, simplify_path(name));
}

int deldir(const char *dir) {
    return syscall(SYS_del, simplify_path(dir));
}

int listdir(const char *dir, char *buf, int len) {
    return syscall(SYS_lsdir, simplify_path(dir), buf, len);
}

int chdir(const char *dir) {
    return syscall(SYS_chdir, simplify_path(dir));
}

int print_cwd_path() {
    return syscall(SYS_pwd);
}