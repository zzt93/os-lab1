#include "kernel/manager/block.h"
#include "adt/d_bit_map.h"
#include "drivers/hal.h"
#include "kernel/manager/FM.h"

D_BIT_MAP()

// a block is 1KB now
const int block_size = 1 << 10;
uint32_t block_map_start;
uint32_t block_start;


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

void init_block() {
    uint32_t size = 0;
    init_bitmap(size);
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

void load_super_block() {
}