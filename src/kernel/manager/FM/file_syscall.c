#include "kernel/message.h"

#include "kernel/manager/inode.h"
#include "kernel/manager/block.h"
#include "kernel/manager/f_dir.h"

/**
   create a regular file
*/
int create_file(Msg *m) {
    // whether it is already exist
    return SUCC;
}

int make_dir(Msg *m) {
}

/**
   create a regular file or a directory
*/
int delete_file(Msg *m) {
    // whether it is exist
    return SUCC;
}

static
void list_file(FTE *fte) {
    iNode node;
    get_node(fte, &node);
    n_dev_read(now_disk, FM, buf, node->off, node->size);
}

int list_dir(Msg *m) {
    char *name = (char *)m.dev_id;
    char *buf = (char *)m.req_pid;
    assert(buf != NULL);
    PCB *aim = (PCB *)m.buf;
    if (name == NULL) {// default file path
        list_file(get_fte(aim, CWD));
    }
    if (name[0] == '/') {// an absolute path
        
    } // a relative path
    else if (name[0] == '.') {
        if
    }

    return SUCC;
}