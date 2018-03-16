#ifndef __FILE_TABLE_H__
#define __FILE_TABLE_H__

#include "kernel/manager/inode.h"

/**
   @see inode.h: File_e
 */
typedef enum {
    FT_DIR,
    FT_PLAIN,
    FT_SOFT_LINK,
    // following has no inode
            PIPE,
    SOCKET,
    BLOCK_DEV,
    CHAR_DEV,
} EFileType;

/**
   if type == CHAR_DEV, `node_off`, `filesize` is meaningless
   if type == FT_DIR, `filesize` is meaningless for the time being
*/
typedef struct {
    // node offset in ramdisk/disk relative to region start
    uint32_t node_off;
    // file cursor offset relative to start of file
    int offset;
    // uniquely identify a device
    int dev_id;
    int ref_count;
    EFileType type;
    // cached value -- needing update
    uint32_t filesize;

    // char file_flags; // rwxr-x---. init it use current user compared with file owner
    // user *owner;
    // group *own_group;
} FTE;

// max number for files that all the processes can open
#define MAX_FILE 128

FTE *add_fte(iNode *node, uint32_t offset);

int free_fte(void *p);

extern FTE *stdin;
extern FTE *stdout;
extern FTE *stderr;
extern FTE *default_cwd;

void init_file_table();

void init_thread_cwd();

#include "kernel/message.h"

size_t write_file(Msg *m);

size_t n_read_file(Msg *m);

#endif /* __FILE_TABLE_H__ */