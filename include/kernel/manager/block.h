#ifndef __BLOCK_H__
#define __BLOCK_H__

// this is actually should be read from super-block
// rather than a macro, which may be changed by using kmalloc
// to allocate an area for bit_map.h
// #define NR_BLOCK (1 << 20)

extern const int block_size;

void load_super_block();

#include "common.h"

extern const int block_size;
extern uint32_t block_map_start;
extern uint32_t block_start;
extern uint32_t block_area_size;

// const -- how many offset can a block store
extern int indirect_datalink_nr;
// const -- how many directory entry can a block store
extern int block_dir_num;

#endif /* __BLOCK_H__ */