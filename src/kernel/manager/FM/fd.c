#include "kernel/message.h"
#include "kernel/process.h"


int first_fd(FDE *start, int value) {
    for (i = 0; i < PROCESS_MAX_; i++) {
        if (is_invalid_fd(aim->fd_table[i])) {
            
        }

    }
}

int open_file(Msg *m) {
    // find that inode
    iNode node = find_node(m);
    // add an entry in system opened file table
    add_fte(node);
    // add a FDE in process fd table

}

int close_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    FDE *fd = aim->fd_table + m.dev_id;
    assert(fd != NULL);
    make_invalid(fd);
    FTE *aim = (FTE *)fd->ft_entry;
    assert(aim != NULL);
    // decrease file table count
    if (aim->ref_count == 1) {
        free_fte(aim);
    }
    aim->ref_count --;
}