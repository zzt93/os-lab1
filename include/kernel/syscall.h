#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "x86/memory.h"
#include "assert.h"

int syscall(int, ...);
void do_syscall(TrapFrame*);

/**
   Add new type system call may need to make clean
   the user program and update with new enum number
   of system call id.
 */
enum {
    SYS_create,
    SYS_make,
    SYS_del,
    SYS_open,
    SYS_read,
    SYS_write,
    SYS_close,
    SYS_lseek,
    SYS_dup,
    SYS_dup2,
    SYS_lsdir,
    SYS_chdir,
    // FM and PM delimiter
    FM_PM,
    // process related system call
    SYS_exit,
    SYS_fork,
    SYS_exec,
    SYS_getpid,
    SYS_waitpid,
    SYS_set_priority,
    SYS_get_priority,
    SYS_add_task_ddl,
    SYS_using_edf,
    // Miscellaneous
    MIS,
    // print in serial terminal
    SYS_print_serial,
    // printf in the screen
    SYS_printf,
    // read line from tty4 -- last tty
    SYS_read_line,
    // wait in seconds now
    SYS_wait,
    SYS_set_timer,
    SYS_wait_timer,
    SYS_timer_start,
    SYS_timer_finished,
    // put prompt -- user name/ prompt
    SYS_prompt,
    // for int 0x80 in sleep method
    // @see ./src/kernel/process/schedule.c
    SLEEP,
};

// PM related system call
int fork();
int exec(int filename, char *args);
int exit();
int getpid();
int waitpid(int wait);
int wait(int time);
int set_priority(Pri_t priority);
int get_priority();
int add_task_ddl(int period);
int put_task_ddl(int period);
int using_edf();

int prompt();

// FM related system call
int open(int filename); // return file descriptor
int read(int fd, uint8_t *buf, int len);
int write(int fd, uint8_t *buf, int len);
int close(int fd);
int lseek(int fd, int offset, int whence);
int dup(int oldfd);
int dup2(int oldfd, int newfd);

// hierachy file system call
int createfile(const char *name);
int makedir(const char *name);
int delfile(const char *name);
int deldir(const char *name);
// name(with path)
int listdir(const char *name, char *buf);
int chdir(const char *name);

int timer_start(int seconds);
int timer_finished();
int set_timer(int seconds);
int wait_timer();

#endif /* __SYSCALL_H__ */