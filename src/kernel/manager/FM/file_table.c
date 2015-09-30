#include "adt/bit_map.h"
#include "kernel/manager/fd.h"
#include "kernel/manager/f_dir.h"
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
    // TODO no block means a device?
    fte->type = ((node->type == NO_BLOCK) ? DEV : REG);
    fte->filesize = node->size;
}

FTE * add_fte(iNode *node, uint32_t offset) {
    // find the first free
    lock();
    int i = first_val(FREE);
    if (i == INVALID) {
        return NULL;
    }
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
    NOINTR;
    // set to a directory -- now is root
    // read it from disk
    inode_t aim = file_path(0, default_cwd_name);
    assert(aim == inode_start);
    n_dev_read(now_disk, FM, (char *)&node, aim, sizeof node);
    default_cwd = add_fte(&node, inode_start);
    NOINTR;
}

int detach_fte(FDE *fd, FTE *fte) {
    make_invalid(fd);
    assert(fte != NULL);
    // decrease file table count
    fte->ref_count --;
    assert(fte->ref_count >= 0);
    if (fte->ref_count == 0) {
        free_fte(fte);
    }
    return SUCC;
}

void init_thread_cwd() {
    int i, thread_num = pcb_size();
    PCB *pcbs[thread_num];
    int res = fetch_all_pcb(pcbs, thread_num);
    assert(res == 0);
    for (i = 0; i < thread_num; i++) {
        init_fd_table(pcbs[i], default_cwd);
    }
}

/**
  TODO
  1. if it is a directory, not permit to write
  2. the size in fte is out of dated
 */
size_t rw_prepare(Msg *m,
    size_t (*rw_block_file)(inode_t, uint32_t, char *buf, int len)) {
    PCB *aim = (PCB *)m->req_pid;
    char *buf = (char *)get_pa(&aim->pdir, (uint32_t)m->buf);
    int fd = m->dev_id;
    if (is_invalid_fd(&aim->fd_table[fd])) {
        m.ret = NO_SUCH;
        return 0;
    }
    // if not specify the list name,
    // using default file path -- current working directory node_off
    FTE *fte = ((FTE *)aim->fd_table[fd].ft_entry);
    inode_t nodeoff = fte->node_off;
    int offset = fte->offset;
    assert(nodeoff >= inode_start);
    return rw_block_file(nodeoff, offset, buf, m->len);
}

size_t write_file(Msg *m) {
    return rw_prepare(m, write_block_file);
}

size_t n_read_file(Msg *m) {
    return rw_prepare(m, read_block_file);
}