#ifndef __FILE_TABLE_H__
#define __FILE_TABLE_H__

#include "kernel/manager/inode.h"

typedef enum {
    PIPE,
    DEV,
    // upper has no inode
    REG,
} Node_e;

typedef struct {
    // node offset in ramdisk/disk relative to init_off
    uint32_t node_off;
    // file cursor offset relative to start of file
    int offset;
    // uniquely identify a device
    int dev_id;
    int ref_count;
    /**
       if type == DEV, node_off, filesize is meaningless
     */
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

#endif /* __FILE_TABLE_H__ */