#ifndef __F_DIR_H__
#define __F_DIR_H__

#define MAX_FILENAME_LEN 28

#include "inode.h"

/**
   Notice:
   if you add more members to Dir_entry
   remember that check whether the sizeof(Dir_entry) is
   still 2**n;
   @see file_syscall.c: test_sizeof_Dir_entry
   @see harddisk/makeimg.py
 */
typedef struct {
    char filename[MAX_FILENAME_LEN];
    // inode offset relative to very beginning of hard disk
    inode_t inode_off;
} Dir_entry;


#define MAX_DIR_DEPTH 16

#define MAX_PATH_LEN (MAX_DIR_DEPTH * sizeof(Dir_entry))

extern const char *const current_dir;
extern const char *const father_dir;
extern const char *const default_cwd_name;

#include "error.h"


uint32_t get_dir_e_off(iNode *dir, inode_t aim);

#include "lib/string.h"

static inline
bool null_filename(const char *name) {
    return name == NULL;
}

static inline
bool filename_too_long(const char *name) {
    return strlen(name) >= MAX_FILENAME_LEN;
}

inode_t file_nodeoff(inode_t cwd, const char *const name);

#include "kernel/message.h"

int list_dir(Msg *m);

int ch_dir(Msg *m);

int delete_file(Msg *m);

int make_dir(Msg *m);

int create_file(Msg *m);


#endif /* __F_DIR_H__ */