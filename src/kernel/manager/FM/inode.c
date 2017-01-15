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
char assert_enum_size[sizeof(ENodeType) == 4 ? 1 : -1];

static inline uint32_t nodei_off(int index) {
    uint32_t new = inode_start + index * inode_size;
    // for first node must be root which is set by python script
    assert(new > inode_start && new < block_start);
    return new;
}

static inline uint32_t node_map_s() {
    return inode_map_start;
}

static inline int off_nodei(uint32_t offset) {
    int gap = offset - inode_start;
    assert(gap > 0 && gap % inode_size == 0);
    return gap / inode_size;
}

/**
   initialize the bit map for inode from ramdisk/disk
   initialize the inode number from ramdisk/disk
   msize -- is in unit of bytes in order to read from now_disk
 */
void init_inode(uint32_t mstart, uint32_t msize, uint32_t start, uint32_t size) {
    lock();
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
    unlock();
    // copy from harddisk
    n_dev_read(now_disk, FM, bits(), mstart, msize);
}

uint32_t inode_alloc() {
    // allocate inode
    int j = first_val(FREE);
    if (j == INVALID) {
        printk(BLUE"\n no more inode to allocate\n"RESET);
        return -1;
    }
    int index = set_val(j, USED);
    // write back to disk
    n_dev_write(now_disk, FM, bits() + index, node_map_s() + index, 1);
    return nodei_off(j);
}

int inode_free(uint32_t offset) {
    // free inode
    int j = off_nodei(offset);
    int index = set_val(j, FREE);
    // write back to disk -- at once???
    return n_dev_write(now_disk, FM, bits() + index, node_map_s() + index, 1);
}

#define FINE 0
#define NO_DATA_BLOCK 1
extern const int default_file_block;
static inline
int invalid_block(iNode *node, int index) {
    if (index < default_file_block) {
        return FINE;
    }
    if (index * block_size < node->size) {
        return FINE;
    }
    return NO_DATA_BLOCK;
}

/**
   @param index -- The index of a block

   NOTICE: try not invoke it directly

   assume that if index is out of bound for a single node,
   the index will always increase
   one by one, otherwise, it will not work.

   return the offset in the disk of `index` block for this node
   (notice not the index for node->index, but the count of
   node's block)
   if that index out of bound, allocate a new block;

   if allocate one block then fail to continue, how to
   free already allocated disk memory. -- solved by
   using macro ALLOC_CHECK
 */
uint32_t get_block(iNode *node, int index) {
    uint32_t res;
    if (index < block_index_range[0]) {// using direct data link
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            ALLOC_CHECK(node->index[index], block_alloc,
                EMPTY_FREE_STATEMENT);
        }
        res = node->index[index];
    } // one level indirect link -- read once
    else if (index < block_index_range[1]) {
        uint32_t first_off = (index - block_index_range[0]) * sizeof(uint32_t);
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            ALLOC_CHECK(res, block_alloc,
                EMPTY_FREE_STATEMENT);
            if (first_off == 0) {// the first time use first-level indirect link
                ALLOC_CHECK(
                    node->index[FIRST_INDIRECT], block_alloc,
                    block_free(res);
                            );
            }
            block_in_range_check(node->index[FIRST_INDIRECT] + first_off);
            // write address of content block to index block
            n_dev_write(now_disk, FM, &res,
                node->index[FIRST_INDIRECT] + first_off, sizeof res);
        } else { // block index in range, just read it
            n_dev_read(now_disk, FM, &res,
                node->index[FIRST_INDIRECT] + first_off, sizeof res);
        }
    } // two level indirect link -- read twice
    else if (index < block_index_range[2]) {
        uint32_t more = index - block_index_range[1];
        // offset on first-level index block which has the second-level index block address
        uint32_t first_off = more / indirect_datalink_nr * sizeof(uint32_t);
        // offset on the second-level index block which has address of content block
        uint32_t second_off = (more % indirect_datalink_nr) * sizeof(uint32_t);
        // second-level index block starting address
        uint32_t second;
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            ALLOC_CHECK(res, block_alloc,
                EMPTY_FREE_STATEMENT);
            if (second_off == 0) { // first time to access it, must be write
                ALLOC_CHECK(second, block_alloc,
                    block_free(res);
                            );
                if (first_off == 0) {
                    ALLOC_CHECK(
                        node->index[SEC_INDIRECT], block_alloc,
                        {
                            block_free(res);
                            block_free(second);
                        }
                        );
                }
                block_in_range_check(node->index[SEC_INDIRECT] + first_off);
                n_dev_write(now_disk, FM, &second,
                    node->index[SEC_INDIRECT] + first_off, sizeof second);
            } else {
                block_in_range_check(node->index[SEC_INDIRECT] + first_off);
                n_dev_read(now_disk, FM, &second,
                    node->index[SEC_INDIRECT] + first_off, sizeof second);
            }
            block_in_range_check(second + second_off);
            n_dev_write(now_disk, FM, &res,
                second + second_off, sizeof res);
        } else {
            n_dev_read(now_disk, FM, &second,
                node->index[SEC_INDIRECT] + first_off, sizeof second);
            n_dev_read(now_disk, FM, &res,
                second + second_off, sizeof res);
        }
    } //three level indirect link -- read three time
    else if (index < block_index_range[3]) {
        uint32_t more = index - block_index_range[2];
        uint32_t first_off = (more / (indirect_datalink_nr * indirect_datalink_nr)) * sizeof(uint32_t);
        uint32_t second_off = (more % (indirect_datalink_nr * indirect_datalink_nr)) / indirect_datalink_nr * sizeof(uint32_t);
        uint32_t thi_off = ((more % (indirect_datalink_nr * indirect_datalink_nr)) % indirect_datalink_nr) * sizeof(uint32_t);
        uint32_t second, thi;
        if (invalid_block(node, index) == NO_DATA_BLOCK) {
            ALLOC_CHECK(res, block_alloc, EMPTY_FREE_STATEMENT);
            if (thi_off == 0) {
                ALLOC_CHECK(thi, block_alloc,
                    block_free(res);
                            );
                if (second_off == 0) {
                    ALLOC_CHECK(second, block_alloc,
                        {
                            block_free(res);
                            block_free(thi);
                        }
                                );
                    if (first_off == 0) {
                        ALLOC_CHECK(node->index[THI_INDIRECT], block_alloc,
                            {
                                block_free(res);
                                block_free(thi);
                                block_free(second);
                            }
                                    );
                    }
                    n_dev_write(now_disk, FM, &second,
                        node->index[THI_INDIRECT] + first_off, sizeof second);
                } else {
                    n_dev_read(now_disk, FM, &second,
                        node->index[THI_INDIRECT] + first_off, sizeof second);
                }
                n_dev_write(now_disk, FM, &thi,
                    second + second_off, sizeof thi);
            } else {
                n_dev_read(now_disk, FM, &thi,
                    second + second_off, sizeof thi);
            }
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
    block_in_range_check(res);
    return res;
}

/**
   Only update the block area(may be block map area if necessary)
   so, for write, have to update inode yourself.

   offset -- the offset in bytes relative to file start,
   for example, read a.txt from the 3 byte of its beginning.
   node -- it store the position for hard disk to read/write
 */
static
size_t rw_file_block(char *buf, iNode *node, uint32_t offset, int len,
    size_t (*n_dev_rw)(int, pid_t, void *, off_t, size_t)) {
    assert(len > 0);
    int block_i = offset / block_size;
    int block_inner_off = offset % block_size;
    uint32_t block_off;
    size_t rw = 0;
    int to_rw = MIN(len, block_size - block_inner_off);
    while (len > rw) {
        block_off = get_block(node, block_i) + block_inner_off;
        if (block_off == NO_MORE_DISK) {
            return rw;
        }

        // to_rw = MIN(len, block_size - block_inner_off) -- make sure block_inner_off
        // become `0` or len == rw, i.e. read/write is over
        block_inner_off = 0;
        // FM as request pid, which should be
        // the owner of buffer
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

   - len can be -1 which refer to as R_LAST_BYTE, means read to last byte
   - len <= len(buf)
 */
size_t read_block_file(int dev_id, inode_t nodeoff, uint32_t offset, char *buf, int len) {
    iNode node;
    // TESTED test change to dev_id -- assert(dev_id == now_disk);
    n_dev_read(dev_id, FM, &node, nodeoff, sizeof node);
    if (len == R_LAST_BYTE) {
        len = node.size;
    }

    if (offset > node.size || offset + len > node.size) {
        return 0;
    }
    int read = rw_file_block(buf, &node, offset, len, n_dev_read);
    assert(len == read);
    return read;
}

/**
   write content to buffer, set right file size
   and extend block if necessary
   offset can be last byte refer to as -1.

   - len >= len(buf)

   area updated:
   - inode map area -- file size
   - block area -- write content
   - block map area(may be need to extend)
 */
size_t write_block_file(int dev_id, inode_t nodeoff, uint32_t offset, char *buf, int len) {
    iNode node;
    n_dev_read(dev_id, FM, &node, nodeoff, sizeof node);
    // temporary assert for testing make/delete dir
    //assert((node.size % sizeof(Dir_entry)) == 0);
    //assert(len % sizeof(Dir_entry) == 0);
    if (offset == W_LAST_BYTE) {
        offset = node.size;
    }
    if (offset > node.size) {
        return 0;
    }
    size_t write = rw_file_block(buf, &node, offset, len, n_dev_write);
    if (write == 0) {
        return write;
    }
    if (offset + write > node.size) {
        node.size = offset + write;
        n_dev_write(now_disk, FM,
            &node, nodeoff, sizeof(iNode));
    }
    return write;
}

/*
   return how many bytes are deleted -- may be no use
size_t del_block_file_content(iNode *file, uint32_t offset, int len) {
    assert(offset + len < file->size);
    size_t to_rw = file->size - offset - len;
    char buf[to_rw];
    size_t read = rw_file_block(buf, file, offset + len, to_rw, n_dev_read);
    size_t write = rw_file_block(buf, file, offset, to_rw, n_dev_write);
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
    rw_file_block((char *)entries, dir, 0, dir->size, n_dev_read);
    int i;
    for (i = 0; i < num_files; i++) {
        if (aim == entries[i].inode_off) {
            return i * sizeof(Dir_entry);
        }
    }
    return -1;
}

size_t del_block_file_dir(inode_t father, inode_t aim) {
    iNode file;
    n_dev_read(now_disk, FM,
        &file, father, sizeof(iNode));
    uint32_t offset = get_dir_e_off(&file, aim);
    // if offset == -1, the following would also fail
    size_t to_rw = sizeof(Dir_entry);
    assert((file.size - offset) % to_rw == 0);
    assert(offset + to_rw <= file.size);
    // more content behind to overwrite it
    size_t read;
    size_t write;
    if (offset + to_rw < file.size) {
        char buf[to_rw];
        // change the content of block area
        // read the last directory and write the aim place
        read = rw_file_block(buf, &file, file.size - to_rw, to_rw, n_dev_read);
        write = rw_file_block(buf, &file, offset, to_rw, n_dev_write);
        assert(read == write && write == to_rw);
    }

    // if using less block
    if ((file.size - to_rw) / block_size < file.size / block_size) {
        // change the block bitmap if necessary
        block_free(file.size / block_size);
    }
    // change the content of inode information(size, )
    file.size -= to_rw;
    n_dev_write(now_disk, FM, &file, father, sizeof(iNode));
    return to_rw;
}
