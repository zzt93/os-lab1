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

#define NO_DATA_BLOCK 1
// the following three invalid block number
// respectively mean:
// no first level link and a data link
#define NO_F_LINK 2
// no first
#define NO_S_LINK 3
#define NO_T_LINK 4
static inline
int invalid_block(iNode *node, int index) {
    return index * block_size >= node->size;
}

/**
   return the offset in the disk of `index` block for this node
   (notice not the index for node->index, but the count of
   node's block)
   if that index out of bound, allocate a new block;
 */
uint32_t get_block(iNode *node, int index) {
    uint32_t res;
    if (index < DATA_LINK_NUM) {// using direct data link
        if (invalid_block(node, index)) {
            node->index[index] = block_alloc();
        }
        return node->index[index];
    } // one level indirect link -- read once
    else if (index < DATA_LINK_NUM + indirect_datalink_nr) {
        
        n_dev_read(now_disk, FM, &res,
            node->index[FIRST_INDIRECT] + sizeof(uint32_t) * (index - DATA_LINK_NUM), sizeof res);
    } // two level indirect link -- read twice
    else if (index < DATA_LINK_NUM +
               indirect_datalink_nr * indirect_datalink_nr) {
        uint32_t more = index - DATA_LINK_NUM - indirect_datalink_nr;
        uint32_t first_off = more / indirect_datalink_nr * sizeof(uint32_t);
        uint32_t second_off = (more % indirect_datalink_nr) * sizeof(uint32_t);
        uint32_t second;
        n_dev_read(now_disk, FM, &second,
            node->index[SEC_INDIRECT] + first_off, sizeof second);
        n_dev_read(now_disk, FM, &res,
            second + second_off, sizeof res);
    } //three level indirect link -- read three time
    else if (index < DATA_LINK_NUM +
               indirect_datalink_nr *
               indirect_datalink_nr *
               indirect_datalink_nr) {
        uint32_t more = index - DATA_LINK_NUM - indirect_datalink_nr * indirect_datalink_nr;
        uint32_t first_off = (more / (indirect_datalink_nr * indirect_datalink_nr)) * sizeof(uint32_t);
        uint32_t second_off = (more % (indirect_datalink_nr * indirect_datalink_nr)) / indirect_datalink_nr * sizeof(uint32_t);
        uint32_t thi_off = ((more % (indirect_datalink_nr * indirect_datalink_nr)) % indirect_datalink_nr) * sizeof(uint32_t);
        uint32_t second, thi;
        n_dev_read(now_disk, FM, &second,
            node->index[THI_INDIRECT] + first_off, sizof second);
        n_dev_read(now_disk, FM, &thi,
            second + second_off, sizeof thi);
        n_dev_read(now_disk, Fm, &res,
            thi + thi_off, sizeof res);
    } else {
        assert(0);
    }
    return res;
}

static
size_t rw_file(char *buf, iNode node, uint32_t offset, int len,
    size_t (*n_dev_rw)(int, pid_t, void *, off_t, size_t)) {
    assert(len > 0);
    int block_i = offset / block_size;
    int block_inner_off = offset % block_size;
    uint32_t block_off = get_block(node, index) + offset % block_size;
    size_t rw = 0;
    int to_rw = MIN(len, block_size - block_inner_off);
    while (len > rw) {
        rw += n_dev_rw(now_disk, FM, buf + rw, block_off, to_rw);
        index ++;
        block_off = get_block(node, index);
        to_rw = MIN(len - rw, block_size);
    }
    return rw;
}

/**
   read file in unit of bytes in range of
   [offset, offset + len) and return how
   many bytes are actually read
   if `offset` is invalid( > size of file),
   return error message
 */
size_t read_file(char *buf, iNode *node, uint32_t offset, int len) {
    if (offset > node->size || offset + len > node->size) {
        return 0;
    }
    int read = rw_file(buf, node, offset, len, n_dev_read);
    assert(len == read);
    return read;
}

/**
   write content to buffer, set right file size
   and extend block if necessary
 */
size_t write_file(iNode *node, uint32_t offset, char *buf, int len) {
    if (offset > node->size) {
        return 0;
    }
    size_t write = rw_file(buf, node, offset, len, n_dev_write);
    node->size += write;
    return write;
}