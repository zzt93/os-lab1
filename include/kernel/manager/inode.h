#ifndef __INODE_H__
#define __INODE_H__

#include "common.h"

/**
   @see file_table.h: Node_e
 */
typedef enum {
    NODE_DIR,
    NODE_PLAIN,
    NODE_SOFT_LINK,
    NOT_INODE,
} ENodeType;

typedef uint32_t block_t;

#define FILE_LINK_NUM 15
// start from 0
#define DATA_LINK_NUM 12
#define FIRST_INDIRECT DATA_LINK_NUM
#define SEC_INDIRECT (FIRST_INDIRECT + 1)
#define THI_INDIRECT (SEC_INDIRECT + 1)

/**
 * inode is the abstraction of file storage, channel to different storage media
   Notice:
   if you add more members to Dir_entry
   remember that check whether the sizeof(Dir_entry) is
   still 2**n;
   @see inode.c: assert_iNode_size
   @see harddisk/makeimg.py
 */
struct INODE {
    //char filename[32];
    // file size in bytes
    size_t size;
    // ram or hda
    int dev_id;
    // store the index of disk block
    uint32_t index[FILE_LINK_NUM];
    // count of hard link
    int link_count;
    ENodeType type;
} __attribute__ ((__aligned__(128)));

typedef struct INODE iNode;

typedef uint32_t inode_t;

extern const int inode_size;
extern uint32_t inode_map_start;
extern uint32_t inode_start;
extern uint32_t inode_area_size;

#include "file_table.h"

uint32_t get_block(iNode *node, int index);

#define W_LAST_BYTE -1
#define R_LAST_BYTE -1

// this one is metaphor for `=` assignment, but for unify read
// and write change it to write like
//size_t read_block_file(char *buf, iNode *node, uint32_t offset, int len);
size_t read_block_file(int dev_id, inode_t nodeoff, uint32_t offset, char *buf, int len);

// write to current file cursor's position
size_t write_block_file(int dev_id, inode_t nodeoff, uint32_t offset, char *buf, int len);

size_t del_block_file_dir(inode_t fileoff, inode_t aim);

uint32_t inode_alloc();

int inode_free(uint32_t);

static inline
int invalid_inode(inode_t off) {
    return off < inode_start;
}

#endif /* __INODE_H__ */