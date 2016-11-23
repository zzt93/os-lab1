#include <kernel/kernel.h>
#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/fd.h"

#include "kernel/manager/fd_ft.h"
#include "kernel/manager/f_dir.h"

#include "kernel/manager/MM_util.h"
#include "kernel/manager/FM.h"
#include "drivers/hal.h"

int first_fd(PCB *start, void *value);

FDE * get_fde(PCB *aim, int i) {
    assert(i >= 0 && i < PROCESS_MAX_FD);
    return aim->fd_table + i;
}


/**
   TODO how to handle link file:

   - find file by name
   - add entry to system opened file table
   - add entry to process fd table
 */
int open_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    PCB *own_name = (PCB *)m->req_pid;
    const char *name = simplify_path(own_name->cwd_path,
                                     (const char *) get_pa(&own_name->pdir, m->dev_id));
    if (null_filename(name)) {
        m->ret = INVALID_FILENAME;
        return INVALID_FD_I;
    }
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    uint32_t node_off = file_path(cwd, name);
    if (node_off < inode_start) {
        m->ret = node_off;
        return INVALID_FD_I;
    }

    iNode node;
    n_dev_read(now_disk, FM,
        &node, node_off, sizeof node);
    // add an entry in system opened file table
    // TODO only adding link target to file table; for reading
    // and writing to it become convenient(no more change)
    FTE *fte = add_fte(&node, node_off);
    int j = first_fd(aim, INVALID_FD);
    if (fte == NULL || invalid_fd_i(j)) {
        m->ret = NO_MORE_MEMORY;
        return INVALID_FD_I;
    }

    // add a FTE in process fd table
    assign_fte(&aim->fd_table[j], fte);
    m->ret = SUCC;
    return j;
}

/**
   - free that process fd
   - free system opened file if necessary
 */
int close_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    int i =  m->dev_id;
    FDE *fd = get_fde(aim, i);
    if (is_invalid_fd(fd)) {
        return INVALID_FD_I;
    }
    FTE *fte = (FTE *)fd->ft_entry;
    detach_fte(fd, fte);

    m->ret = SUCC;
    return i;
}

/**
   On success, these system calls return the new descriptor.
   On error, -1 is returned
 */
int dup_file(Msg *m) {
    int i = m->dev_id;
    PCB *aim = (PCB *)m->buf;
    FDE *src = get_fde(aim, i);
    if (is_invalid_fd(src)) {
        return INVALID_FD_I;
    }
    int j = first_fd(aim, INVALID_FD);
    if (j == INVALID_FD_I) {
        return j;
    }
    assign_fd(get_fde(aim, j), src);

    m->ret = SUCC;
    return j;
}

/**
   On success, these system calls return the new descriptor.
   On error, -1 is returned
 */
int dup2_file(Msg *m) {
    PCB *aim = (PCB *)m->i[2];
    int i = m->i[0];
    FDE *src = get_fde(aim, i);
    if (is_invalid_fd(src)) {
        return INVALID_FD_I;
    }
    int j = m->i[1];
    if (!is_invalid_fd(get_fde(aim, j))) {
        assert(m->i[1] == m->dev_id);
        close_file(m);
    }
    assign_fd(get_fde(aim, j), src);

    m->ret = SUCC;
    return j;
}
