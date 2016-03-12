#include "adt/bit_map.h"
#include "kernel/manager/fd.h"
#include "kernel/manager/f_dir.h"
#include "kernel/manager/manager.h"
#include "kernel/manager/dev_file.h"

#include "lib/string.h"
#include "kernel/process.h"
#include "drivers/hal.h"

FTE *stdin;
FTE *stdout;
FTE *stderr;
FTE *default_cwd;

BIT_MAP(MAX_FILE)

static FTE file_table[MAX_FILE];

static
void add_node_to_fte(FTE *fte, iNode *node, uint32_t offset) {
    fte->offset = 0;
    fte->dev_id = node->dev_id;
    fte->ref_count = 0;
    assert(node->type != NOT_INODE);
    fte->type = node->type;
    fte->node_off = offset;
    switch(fte->type) {
        case FT_DIR:
            // TODO add file size for directory here?
            break;
        case FT_PLAIN:
            fte->filesize = node->size;
            break;
        default:
            assert(0);
    }
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
    add_node_to_fte(aim, node, offset);
    set_val(i, USED);
    unlock();
    return aim;
}

/**
   Add file type: fifo -- pipe, socket, character device, block device
 */
FTE * add_special_file_to_fte(uint32_t offset, int dev_id, Node_e type) {
    // find the first free
    lock();
    int i = first_val(FREE);
    if (i == INVALID) {
        return NULL;
    }
    // initialize fte
    FTE *fte = file_table + i;
    fte->node_off = -1;
    fte->offset = offset;
    fte->dev_id = dev_id;
    fte->ref_count = 0;
    fte->type = type;
    fte->filesize = -1;

    set_val(i, USED);
    unlock();
    return fte;
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
    int dev_id = d_ttyi[NOW_TERMINAL];
    // offset for a device is meaningless?
    stdin = add_special_file_to_fte(-1, dev_id, CHAR_DEV);
    stdout = add_special_file_to_fte(-1, dev_id, CHAR_DEV);
    stderr = add_special_file_to_fte(-1, dev_id, CHAR_DEV);
    NOINTR;
    // set to a directory -- now is root
    // read it from disk
    // TODO using "/" as default cwd for the time being
    inode_t aim = file_path(0, default_cwd_name);
    assert(aim == inode_start);
    iNode node;
    n_dev_read(now_disk, FM, (char *)&node, aim, sizeof node);
    // for it is a directory, so the size for it is meaningless
    default_cwd = add_fte(&node, aim);
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
    assert(res == thread_num);
    for (i = 0; i < thread_num; i++) {
        init_fd_table(pcbs[i], default_cwd);
    }
}

/**
  1. if it is a directory, not permit to write -- already add check
  2. the size of cwd in fte is out-dated -- never read size of
  directory in fte for its size is already set invalid
 */
size_t rw_prepare(Msg *m,
    size_t (*rw_block_file)(int, inode_t, uint32_t, char *buf, int len)) {
    PCB *aim = (PCB *)m->req_pid;
    char *buf = (char *)get_pa(&aim->pdir, (uint32_t)m->buf);
    int fd = m->dev_id;
    if (is_invalid_fd(&aim->fd_table[fd])) {
        m->ret = NO_SUCH;
        return 0;
    }
    // if not specify the list name,
    // using default file path -- current working directory node_off
    FTE *fte = ((FTE *)aim->fd_table[fd].ft_entry);
    // read and write directory should not through this method
    // write: user can't write to directory
    // read: user should call list_dir
    if (fte->type != FT_PLAIN) {
        switch(fte->type) {
            case FT_DIR:
                m->ret = IS_DIR;
                return 0;
            case CHAR_DEV:
                m->ret = SUCC;
                return write_char_dev_file(fte->dev_id, buf, m->len);
            default:
                // not implement yet
                assert(0);
        }
    }
    // just a plain file
    inode_t nodeoff = fte->node_off;
    int offset = fte->offset;
    assert(nodeoff >= inode_start);
    m->ret = SUCC;
    // update cursor offset in the file table entry after reading/writing
    int rw_byte = rw_block_file(fte->dev_id, nodeoff, offset, buf, m->len);
    fte->offset += rw_byte;
    // TODO @see update cached file of file table
    if (rw_block_file == write_block_file) {
        fte->filesize += rw_byte;
    }

    return rw_byte;
}

size_t write_file(Msg *m) {
    return rw_prepare(m, write_block_file);
}

size_t n_read_file(Msg *m) {
    return rw_prepare(m, read_block_file);
}