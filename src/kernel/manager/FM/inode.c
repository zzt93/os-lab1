#include "kernel/manager/inode.h"
#include "drivers/hal.h"

#include "kernel/manager/FM.h"
#include "adt/d_bit_map.h"

D_BIT_MAP();

const int inode_size = sizeof(iNode);
uint32_t inode_map_start;
uint32_t inode_start;
uint32_t inode_area_size;

char assert_iNode_size[sizeof(iNode) % 128 == 0 ? 1 : -1];
char assert_enum_size[sizeof(File_e) == 4 ? 1 : -1];

static inline uint32_t nodei_off(int index) {
    return inode_start +
        index * inode_size;
}

/**
   for the offset in harddisk is in unit of
   bytes, so all the offset have to in bytes
 */
static inline uint32_t node_mapi_off(int index) {
    return inode_map_start +
        index / sizeof(uint8_t);
}

static inline int off_nodei(uint32_t offset) {
    int gap = offset - inode_start;
    assert(gap > 0 && gap % inode_size);
    return gap / inode_size;
}

/**
   initialize the bit map for inode from ramdisk/disk
   initialize the inode number from ramdisk/disk
 */
void init_inode(uint32_t mstart, uint32_t msize, uint32_t start, uint32_t size) {
    inode_map_start = mstart;
    init_bitmap(msize);
    inode_start = start;
    inode_area_size = size;
    // copy from harddisk
    n_dev_read(now_disk, FM, bits(), mstart, msize);
}

uint32_t inode_alloc() {
    // allocate inode
    int j = first_val(FREE);
    if (j == INVALID) {
        return -1;
    }
    int index = set_val(j, USED);
    // write back to disk
    n_dev_write(now_disk, FM, bits() + index, node_mapi_off(j), 1);
    return nodei_off(j);
}

int inode_free(uint32_t offset) {
    // free inode
    int j = off_nodei(offset);
    int index = set_val(j, FREE);
    // write back to disk -- at once???
    return n_dev_write(now_disk, FM, bits() + index, node_mapi_off(j), 1);
}


void get_node(FTE *fte, iNode *node) {
    n_dev_read(now_disk, FM, node, node_off, sizeof(iNode));
}