#include "kernel/manager/inode.h"
#include "drivers/hal.h"

#include "adt/bit_map.h"

BIT_MAP(NR_INODE);

/**
   initialize the bit map for inode from ramdisk/disk
   initialize the inode number from ramdisk/disk
 */
void init_inode() {
}

int inode_alloc() {
    // allocate inode
    int j = first_val(FREE);
    if (j == INVALID) {
        return -1;
    }
    set_val(j, USED);

    // write back to disk
    return 1;
}

int inode_free() {
    // free inode
    // write back to disk -- at once???
    return 1;
}