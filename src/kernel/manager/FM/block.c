#include "kernel/manager/block.h"
#include "kernel/manager/f_dir.h"
#include "adt/d_bit_map.h"
#include "drivers/hal.h"
#include "kernel/manager/FM.h"

D_BIT_MAP()

// a block is 1KB now
const int block_size = 1 << 10;
uint32_t block_map_start;
uint32_t block_start;
uint32_t block_area_size;


static inline uint32_t blocki_offset(int index) {
    return block_start +
        index * block_size;
}

static inline uint32_t block_mapi_off(int index) {
    return block_map_start +
        index / sizeof(uint8_t);
}

static inline int offset_blocki(uint32_t offset) {
    int gap = offset - block_start;
    assert(gap > 0 && gap % block_size == 0);
    return gap / block_size;
}

int indirect_datalink_nr;
int block_dir_num;
void init_block(uint32_t mstart, uint32_t msize, uint32_t start, uint32_t size) {
    block_map_start = mstart;
    // change to original number of block
    init_bitmap(msize * BYTES_BITS);
    block_start = start;
    block_area_size = size;
    // copy from harddisk
    n_dev_read(now_disk, FM, bits(), mstart, msize);
    indirect_datalink_nr = block_size / sizeof(int);
    block_dir_num = block_size / sizeof(Dir_entry);
}

uint32_t block_alloc() {
    int j = first_val(FREE);
    if (j == INVALID) {
        return -1;
    }
    int index = set_val(j, USED);
    // write back to disk
    n_dev_write(now_disk, FM, bits() + index, block_mapi_off(j), 1);
    return blocki_offset(j);
}

int block_free(uint32_t offset) {
    int j = offset_blocki(offset);
    int index = set_val(j, FREE);
    // write back to disk
    return n_dev_write(now_disk, FM, bits() + index, block_mapi_off(j), 1);
}


void init_inode(uint32_t mstart, uint32_t msize, uint32_t start, uint32_t size);

uint32_t super_start = 1368064;
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
}
