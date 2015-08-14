#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/fd.h"

#include "kernel/manager/fd_ft.h"

int first_fd(PCB *start, void *value);

FDE * get_fde(PCB *aim, int i) {
    assert(i >= 0 && i < PROCESS_MAX_FD);
    return aim->fd_table + i;
}

uint32_t find_node(Msg *, iNode *);

int open_file(Msg *m) {
    // find that inode
    /*
    iNode node;
    uint32_t offset = find_node(m, &node);
    // add an entry in system opened file table
    FTE *fte = add_fte(&node, offset);
    // add a FDE in process fd table
    PCB *aim = (PCB *)m->buf;
    int j = first_fd(aim, INVALID_FD);
    assign_fte(aim->fd_table[j].ft_entry, fte);
    */
    return 1;
}

int close_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    int i =  m->dev_id;
    FDE *fd = get_fde(aim, i);
    if (is_invalid_fd(fd)) {
        return -1;
    }
    FTE *fte = (FTE *)fd->ft_entry;
    detach_fte(fd, fte);
    return 1;
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
        return -1;
    }
    int j = first_fd(aim, INVALID_FD);
    if (j == INVALID_FD_I) {
        return j;
    }
    assign_fd(get_fde(aim, j), src);
    return j;
}

/**
   On success, these system calls return the new descriptor.
   On error, -1 is returned
 */
int dup2_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    int i = m->i[0];
    FDE *src = get_fde(aim, i);
    if (is_invalid_fd(src)) {
        return -1;
    }
    int j = m->i[1];
    if (!is_invalid_fd(get_fde(aim, j))) {
        assert(m->i[1] == m->dev_id);
        close_file(m);
    }
    assign_fd(get_fde(aim, j), src);
    return SUCC;
}

