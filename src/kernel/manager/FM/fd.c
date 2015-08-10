#include "kernel/message.h"
#include "kernel/process.h"


int open_file(Msg *m) {
    int file = m->ebx;
    // add a FDE in process fd table
    // add an entry in system opened file table
}

int close_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    FDE *fd = aim->fd_table + m.dev_id;
    assert(fd != NULL);
    assert(fd->fd == m.dev_id);
    assert(fd->st_entry != NULL);
    // decrease system opened file
    
}