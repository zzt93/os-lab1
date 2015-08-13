#ifndef __INODE_H__
#define __INODE_H__

#include "common.h"

typedef enum {
    NOT_NODE,
    DIR,
    PLAIN,
} File_e;

typedef uint32_t block_t;

#define FILE_LINK_NUM 15

typedef struct {
	//char filename[32];
    // file size
	size_t size;
    // ram or hda
    int dev_id;
	uint32_t index[FILE_LINK_NUM];
    int link_count;
    File_e type;
} iNode __attribute__ ((aligned (128)));

#endif /* __INODE_H__ */