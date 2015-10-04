#ifndef __FILE_TABLE_H__
#define __FILE_TABLE_H__

#include "kernel/manager/inode.h"

typedef enum {
    FT_DIR,
    FT_PLAIN,
    // following has no inode
    PIPE,
    DEV,
} Node_e;

/**
   if type == DEV, node_off, filesize is meaningless
   if type == FT_DIR, filesize if meaningless
*/
typedef struct {
    // node offset in ramdisk/disk relative to region node
    uint32_t node_off;
    // file cursor offset relative to start of file
    int offset;
    // uniquely identify a device
    int dev_id;
    int ref_count;
    Node_e type;
    // cached value -- needing update
    uint32_t filesize;
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