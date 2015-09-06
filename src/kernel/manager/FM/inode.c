#include "kernel/manager/inode.h"
#include "drivers/hal.h"

#include "kernel/manager/FM.h"
#include "kernel/manager/block.h"
#include "kernel/manager/f_dir.h"
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
   msize -- is in unit of bytes in order to read from now_disk
 */
void init_inode(uint32_t mstart, uint32_t msize, uint32_t start, uint32_t size) {
    inode_map_start = mstart;
    // change to original number of inode
    /*
      @bug-fixed: allocating less array causing array out
      of bound.
      for the old wrong size, it will
      allocate less space than needed, when read from
      FM and then write to bitmap, it will overlap
      the head info of next allocated memory block, which
      cause `kmalloc()` assert fail.
    */
    init_bitmap(msize * BYTES_BITS);
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

#define FINE 0
#define NO_DATA_BLOCK 1
static inline
int invalid_block(iNode *node, int index) {
    if (index * block_size < node->size) {
        return FINE;
    }
    return NO_DATA_BLOCK;
}

/**
   NOTICE: try not invoke it directly

   assume that if index is out of bound for a single node,
   the index will always increase
   one by one, otherwise, it will not work.

   return the offset in the disk of `index` block for this node
   (notice not the index for node->index, but the count of
   node's block)
   if that index out of bound, allocate a new block;

 */
uint32_t get_block(iNode *node, int index) {
    uint32_t res;
    if (index < DATA_LINK_NUM) {// using direct data link
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            node->index[index] = block_alloc();
        }
        return node->index[index];
    } // one level indirect link -- read once
    else if (index < DATA_LINK_NUM + indirect_datalink_nr) {
        uint32_t first_off = (index - DATA_LINK_NUM) * sizeof(uint32_t);
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            if (first_off == 0) {
                node->index[FIRST_INDIRECT] = block_alloc();
            }
            res = block_alloc();
            n_dev_write(now_disk, FM, &res,
                node->index[FIRST_INDIRECT] + first_off, sizeof res);
        } else {
            n_dev_read(now_disk, FM, &res,
                node->index[FIRST_INDIRECT] + first_off, sizeof res);
        }
    } // two level indirect link -- read twice
    else if (index < DATA_LINK_NUM +
               indirect_datalink_nr * indirect_datalink_nr) {
        uint32_t more = index - DATA_LINK_NUM - indirect_datalink_nr;
        uint32_t first_off = more / indirect_datalink_nr * sizeof(uint32_t);
        uint32_t second_off = (more % indirect_datalink_nr) * sizeof(uint32_t);
        uint32_t second;
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            if (second_off == 0) { // first time to access it, must be write
                if (first_off == 0) {
                    node->index[SEC_INDIRECT] = block_alloc();
                }
                second = block_alloc();
                n_dev_write(now_disk, FM, &second,
                    node->index[SEC_INDIRECT] + first_off, sizeof second);
            } else {
                n_dev_read(now_disk, FM, &second,
                    node->index[SEC_INDIRECT] + first_off, sizeof second);
            }
            res = block_alloc();
            n_dev_write(now_disk, FM, &res,
                second + second_off, sizeof res);
        } else {
            n_dev_read(now_disk, FM, &second,
                node->index[SEC_INDIRECT] + first_off, sizeof second);
            n_dev_read(now_disk, FM, &res,
                second + second_off, sizeof res);
        }
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
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            if (thi_off == 0) {
                if (second_off == 0) {
                    if (first_off == 0) {
                        node->index[THI_INDIRECT] = block_alloc();
                    }
                    second = block_alloc();
                    n_dev_write(now_disk, FM, &second,
                        node->index[THI_INDIRECT] + first_off, sizeof second);
                } else {
                    n_dev_read(now_disk, FM, &second,
                        node->index[THI_INDIRECT] + first_off, sizeof second);
                }
                thi = block_alloc();
                n_dev_write(now_disk, FM, &thi,
                    second + second_off, sizeof thi);
            } else {
                n_dev_read(now_disk, FM, &thi,
                    second + second_off, sizeof thi);
            }
            res = block_alloc();
            n_dev_write(now_disk, FM, &res,
                thi + thi_off, sizeof res);
        } else {
            n_dev_read(now_disk, FM, &second,
                node->index[THI_INDIRECT] + first_off, sizeof second);
            n_dev_read(now_disk, FM, &thi,
                second + second_off, sizeof thi);
            n_dev_read(now_disk, FM, &res,
                thi + thi_off, sizeof res);
        }
    } else {
        assert(0);
    }
    return res;
}

/**
   offset -- the offset relative to file start, for offset relative
   to the start of whole image is meaning less for a file.
 */
static
size_t rw_file(char *buf, iNode *node, uint32_t offset, int len,
    size_t (*n_dev_rw)(int, pid_t, void *, off_t, size_t)) {
    assert(len > 0);
    int block_i = offset / block_size;
    int block_inner_off = offset % block_size;
    uint32_t block_off;
    size_t rw = 0;
    int to_rw = MIN(len, block_size - block_inner_off);
    while (len > rw) {
        block_off = get_block(node, block_i) + block_inner_off;
        // to_rw = MIN(len, block_size - block_inner_off) -- make sure block_inner_off
        // become `0` or len == rw, i.e. read/write is over
        block_inner_off = 0;
        rw += n_dev_rw(now_disk, FM, buf + rw, block_off, to_rw);
        block_i ++;
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
size_t read_block_file(iNode *node, uint32_t offset, char *buf, int len) {
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
size_t write_block_file(iNode *node, uint32_t offset, char *buf, int len) {
    if (offset > node->size) {
        return 0;
    }
    size_t write = rw_file(buf, node, offset, len, n_dev_write);
    node->size += write;
    return write;
}

/*
   return how many bytes are deleted -- may be no use
size_t del_block_file_content(iNode *file, uint32_t offset, int len) {
    assert(offset + len < file->size);
    size_t to_rw = file->size - offset - len;
    char buf[to_rw];
    size_t read = rw_file(buf, file, offset + len, to_rw, n_dev_read);
    size_t write = rw_file(buf, file, offset, to_rw, n_dev_write);
    // if using less block
    if ((node->size - len) / block_size < node->size / block_size) {
        block_free(node->size / block_size);
    }
    assert(read == write && write == len);
    file->size -= len;
    return len;
}
*/

/**
   return the offset of the directory entry whose offset
   is `aim`
 */
uint32_t get_dir_e_off(iNode *dir, inode_t aim) {
    int num_files = dir->size / sizeof(Dir_entry);
    assert(dir->size % sizeof(Dir_entry) == 0);
    Dir_entry entries[num_files];
    rw_file((char *)entries, dir, 0, dir->size, n_dev_read);
    int i;
    for (i = 0; i < num_files; i++) {
        if (aim == entries[i].inode_off) {
            return i * sizeof(Dir_entry);
        }
    }
    return -1;
}

size_t del_block_file_dir(iNode *file, inode_t aim) {
    uint32_t offset = get_dir_e_off(file, aim);
    // if offset == -1, the following would also fail
    size_t to_rw = sizeof(Dir_entry);
    assert((file->size - offset) % to_rw == 0);
    assert(offset + to_rw <= file->size);
    char buf[to_rw];
    size_t read = rw_file(buf, file, file->size - to_rw, to_rw, n_dev_read);
    size_t write = rw_file(buf, file, offset, to_rw, n_dev_write);
    // if using less block
    if ((file->size - to_rw) / block_size < file->size / block_size) {
        block_free(file->size / block_size);
    }
    assert(read == write && write == to_rw);
    file->size -= to_rw;
    return to_rw;
}
