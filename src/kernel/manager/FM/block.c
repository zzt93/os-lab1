#include "kernel/manager/block.h"
#include "kernel/manager/f_dir.h"
#include "adt/d_bit_map.h"
#include "drivers/hal.h"
#include "kernel/manager/FM.h"

#include "lib/math.h"

D_BIT_MAP()

// a block is 1KB now
const int block_size = 1 << 10;
uint32_t block_map_start;
uint32_t block_start;
uint32_t block_area_size;


static inline uint32_t blocki_offset(int index) {
    uint32_t off = block_start + index * block_size;
    assert(off > block_start && off < block_start + block_area_size);
    return off;
}

static inline uint32_t block_map_s() {
    return block_map_start;
}

static inline int offset_blocki(uint32_t offset) {
    int gap = offset - block_start;
    assert(gap > 0 && gap % block_size == 0);
    return gap / block_size;
}

int indirect_datalink_nr;
int block_dir_num;
#define BLOCK_INDEX_DEEPTH 3
// add one for the direct block not have index
int block_index_range[BLOCK_INDEX_DEEPTH + 1];
void init_block(uint32_t mstart, uint32_t msize, uint32_t start, uint32_t size) {
    lock();
    block_map_start = mstart;
    // change to original number of block
    init_bitmap(msize * BYTES_BITS);
    block_start = start;
    block_area_size = size;
    indirect_datalink_nr = block_size / sizeof(int);
    int i;
    block_index_range[0] = DATA_LINK_NUM;
    for (i = 1; i < BLOCK_INDEX_DEEPTH + 1; i++) {
        block_index_range[i] = block_index_range[i - 1] + pow(indirect_datalink_nr, i);
    }

    unlock();
    // copy from harddisk
    n_dev_read(now_disk, FM, bits(), mstart, msize);
    block_dir_num = block_size / sizeof(Dir_entry);
}

uint32_t block_alloc() {
    int j = first_val(FREE);
    if (j == INVALID) {
        printk(BLUE"\n no more block to allocate\n"RESET);
        return INVALID_ALLOC;
    }
    int index = set_val(j, USED);
    // write back to disk
    n_dev_write(now_disk, FM, bits() + index, block_map_s() + index, 1);
    return blocki_offset(j);
}

/**
   offset -- block's offset
 */
int block_free(uint32_t offset) {
    int j = offset_blocki(offset);
    int index = set_val(j, FREE);
    // write back to disk -- change the block map area
    return n_dev_write(now_disk, FM, bits() + index, block_map_s() + index, 1);
}


void init_inode(uint32_t mstart, uint32_t msize, uint32_t start, uint32_t size);

/**
   Notice:
   not change the following line
   @see harddisk/update_super_block.sh
 */
const uint32_t super_start = 1500160;

#define SUPER_BUF 512
void load_super_block() {
    char buf[SUPER_BUF];
    //n_dev_read(now_disk, FM, buf, super_start, SUPER_BUF);
    n_dev_read(now_disk, FM, buf, super_start, sizeof(uint32_t) * 8);
    // the following may change if we save more info in the super
    // block
    uint32_t *b = (uint32_t *)buf;
    // @checked the super block content is the same with
    // the parameter in makeimg.py
    init_inode(b[0], b[1], b[4], b[5]);
    init_block(b[2], b[3], b[6], b[7]);
    assert(inode_start + inode_area_size == block_start);
    assert(b[0] + b[1] == b[2]);
    assert(b[2] + b[3] == b[4]);
    assert(b[4] + b[5] == b[6]);
}
