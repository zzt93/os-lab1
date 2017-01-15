#ifndef __FM_H__
#define __FM_H__

#include "common.h"
#include "kernel/message.h"

enum {
    FM_ram_read,
    FM_read,
    FM_write,
    FM_open,
    FM_close,
    FM_dup,
    FM_dup2,
    FM_lseek,
    FM_create,
    FM_make,
    FM_del,
    FM_lsdir,
    FM_chdir,
};

void init_FM();
size_t ram_read_file(Msg*);
void init_file_system();

extern int FM;

const char *simplify_path(const char *cwd, const char *path);
int is_absolute_path(const char *path);

#include "fd.h"

#endif /* __FM_H__ */