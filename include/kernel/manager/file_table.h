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
    PIPE, // named pipe, FIFO file
    //-------- following has no inode ------
    SOCKET,
    BLOCK_DEV, //These files are hardware files most of them are present in /dev
    CHAR_DEV, //A serial stream of input or output: terminal
} EFileType;

/**
   if type == CHAR_DEV, `node_off`, `offset`, `filesize` is meaningless
   if type == FT_DIR, `filesize` is meaningless for the time being
   if type == SOCKET, `node_off` is a pointer to socket, `offset` & `filesize` is meaningless

   @see socket
*/
typedef struct {
    // node offset in ramdisk/disk relative to region start
    uint32_t node_off;
    // file cursor offset relative to start of file
    int offset;
    uint32_t filesize; // TODO cached value -- needing update

    // ------------------- all file shared property -------------

    int dev_id; // uniquely identify a device
    int ref_count;
    EFileType type;

    // char file_flags; // rwxr-x---. init it use current user compared with file owner
    // user *owner;
    // group *own_group;
} FTE;

// max number for files that all the processes can open
#define MAX_FILE 128

FTE * add_fte(iNode *node, uint32_t offset);
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