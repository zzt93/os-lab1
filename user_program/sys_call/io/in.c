#include "kernel/syscall.h"
#include "shell.h"


int read_line(char *str, int capacity) {
    return syscall(SYS_read_line, str, capacity);
}

// @Deprecated
//int open(int filename) {
//    return syscall(SYS_open, filename);
//}

int read(int fd, uint8_t *buf, int len) {
    return syscall(SYS_read, fd, buf, len);
}

int write(int fd, uint8_t *buf, int len) {
    return syscall(SYS_write, fd, buf, len);
}

int close(int fd) {
    return syscall(SYS_close, fd);
}

int lseek(int fd, int offset, int whence) {
    return syscall(SYS_lseek, fd, offset, whence);
}

int dup(int oldfd) {
    return syscall(SYS_dup, oldfd);
}

int dup2(int oldfd, int newfd) {
    return syscall(SYS_dup2, oldfd, newfd);
}
