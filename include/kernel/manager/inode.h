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
// this is actually should be read from super-block
// rather than a macro, which may be changed by using kmalloc
// to allocate an area for bit_map.h
// #define NR_INODE (1 << 12)

typedef struct {
	//char filename[32];
    // file size
	size_t size;
    // ram or hda
    int dev_id;
    // store the index of disk block
	uint32_t index[FILE_LINK_NUM];
    // count of hard link
    int link_count;
    File_e type;
} iNode __attribute__ ((aligned (128)));


#endif /* __INODE_H__ */