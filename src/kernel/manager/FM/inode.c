#include "kernel/manager/inode.h"

//BIT_MAP()

/**
   initialize the bit map for inode from ramdisk/disk
 */
void init_inode() {
}

int inode_alloc() {
    // allocate inode
    // write back to disk
    return 1;
}

int inode_free() {
    // free inode
    // write back to disk -- at once???
    return 1;
}