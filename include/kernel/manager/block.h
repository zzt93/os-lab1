#ifndef __BLOCK_H__
#define __BLOCK_H__

// TODO this is actually should be read from super-block
// rather than a macro, which may be changed by using kmalloc
// to allocate an area for bit_map.h
// #define NR_BLOCK (1 << 20)

extern const int block_size;


#endif /* __BLOCK_H__ */