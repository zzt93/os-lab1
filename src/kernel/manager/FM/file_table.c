#include "adt/bit_map.h"
#include "kernel/manager/fd.h"
#include "kernel/manager/manager.h"

#include "lib/string.h"
#include "kernel/process.h"
#include "drivers/hal.h"

FTE *stdin;
FTE *stdout;
FTE *stderr;
FTE *default_cwd;

BIT_MAP(MAX_FILE)

static FTE file_table[MAX_FILE];

void fill_fte(FTE *fte, iNode *node, uint32_t offset) {
    fte->node_off = offset;
    fte->offset = 0;
    fte->dev_id = node->dev_id;
    fte->ref_count = 0;
    // TODO pointing an inode means not device?
    fte->type = ((node->type == NO_BLOCK) ? DEV : REG);
    fte->filesize = node->size;
}

FTE * add_fte(iNode *node, uint32_t offset) {
    // find the first free
    lock();
    int i = first_val(FREE);
    assert(i != INVALID);
    // initialize fte
    FTE *aim = file_table + i;
    fill_fte(aim, node, offset);
    set_val(i, USED);
    unlock();
    return aim;
}

int free_fte(void *p) {
    assert(p != NULL);
    FTE *fte = (FTE *)p;
    lock();
    set_val(fte - file_table, FREE);
    unlock();
    return 1;
}

FDE * get_fde(PCB *, int );

FTE * get_fte(PCB *aim, int fd) {
    FDE *fde = get_fde(aim, fd);
    return (FTE *)fde->ft_entry;
}

/**
   TODO init inode bit map; init block bit map
   init by super block(start and size of each region)
   initialize STD*** in file table
 */
void init_file_table() {
    iNode node;
    node.size = -1;
    node.dev_id = d_ttyi[NOW_TERMINAL];
    memset(node.index, 0, sizeof(uint32_t) * FILE_LINK_NUM);
    node.link_count = 0;
    node.type = NO_BLOCK;
    stdin = add_fte(&node, -1);
    stdout = add_fte(&node, -1);
    stderr = add_fte(&node, -1);
    // set to a directory -- now is root
    // read it from disk
    n_dev_read(now_disk, FM, (char *)&node, inode_start, sizeof node);
    default_cwd = add_fte(&node, inode_start);
}

int detach_fte(FDE *fd, FTE *fte) {
    make_invalid(fd);
    assert(fte != NULL);
    // decrease file table count
    if (fte->ref_count == 1) {
        free_fte(fte);
    }
    fte->ref_count --;
    return SUCC;
}