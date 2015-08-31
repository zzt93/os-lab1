#ifndef __INODE_H__
#define __INODE_H__

#include "common.h"

typedef enum {
    NO_BLOCK,
    DIR,
    PLAIN,
} File_e;

typedef uint32_t block_t;

#define FILE_LINK_NUM 15
// start from 0
#define DATA_LINK_NUM 12
#define FIRST_INDIRECT DATA_LINK_NUM
#define SEC_INDIRECT (FIRST_INDIRECT + 1)
#define THI_INDIRECT (SEC_INDIRECT + 1)


struct INODE{
	//char filename[32];
    // file size in bytes
	size_t size;
    // ram or hda
    int dev_id;
    // store the index of disk block
	uint32_t index[FILE_LINK_NUM];
    // count of hard link
    int link_count;
    File_e type;
} __attribute__ ((__aligned__(128)));

typedef struct INODE iNode;


extern const int inode_size;
extern uint32_t inode_map_start;
extern uint32_t inode_start;
extern uint32_t inode_area_size;

#include "file_table.h"

uint32_t get_block(iNode *node, int index);

size_t read_file(char *buf, iNode *node, int offset, int len);

#endif /* __INODE_H__ */