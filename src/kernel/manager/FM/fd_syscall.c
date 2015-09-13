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


int open_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (invalid_filename(name)) {
        return INVALID_FD_I;
    }
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    uint32_t node_off = file_path(cwd, name);
    iNode node;
    n_dev_read(now_disk, FM,
        &node, node_off, sizeof node);
    // add an entry in system opened file table
    FTE *fte = add_fte(&node, node_off);
    // add a FDE in process fd table
    int j = first_fd(aim, INVALID_FD);
    assign_fte(aim->fd_table[j].ft_entry, fte);
    m->ret = SUCC;
    return j;
}

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
