#include "adt/bit_map.h"
#include "drivers/hal.h"

BIT_MAP(NR_BLOCK)

// a block is 1KB now
const int block_size = 1 << 10;

void init_block() {
}

int block_alloc() {
    int j = first_val(FREE);
    if (j == INVALID) {
        return -1;
    }
    set_val(j, USED);
    // write back to disk
    return 1;
}

int block_free() {
    // write back to disk
    return 1;
}