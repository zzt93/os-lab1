#include "kernel/message.h"
#include "drivers/hal.h"
#include "kernel/process.h"
#include "kernel/manager/MM_util.h"
#include "kernel/manager/FM.h"

#include "kernel/manager/inode.h"
#include "kernel/manager/block.h"
#include "kernel/manager/f_dir.h"

#include "lib/string.h"

char test_sizeof_Dir_entry[sizeof(Dir_entry) % 32 == 0 ? 1 : 1];

const char *const no_such = "No such file or directory";
int no_such_size;

const char *const current_dir = ".";
const char *const father_dir = "..";

/**
   test whether a directory contain a file by check the
   name list it store
   @return:
   0 -- not contain
   node_offset of the file with `name`
 */
static int contain_file(inode_t node_off, char *name) {
    iNode node;
    n_dev_read(now_disk, FM, &node, node_off, sizeof node);
    assert(node.type == DIR);
    int size = node.size;
    assert(size % sizeof(Dir_entry) == 0);
    Dir_entry dirs[block_dir_num];
    int i = 0, j;
    uint32_t block_off = get_block(&node, i);
    while (size > block_size) {
        n_dev_read(now_disk, FM, dirs, block_off, block_size);
        for (j = 0; j < block_dir_num; j++) {
            if (strcmp(dirs[j].filename, name) == 0) {
                return dirs[j].inode_off;
            }
        }
        i ++;
        block_off = get_block(&node, i);
        size -= block_size;
    }
    n_dev_read(now_disk, FM, dirs, block_off, size);
    for (j = 0; j < size / sizeof(Dir_entry); j++) {
        if (strcmp(dirs[j].filename, name) == 0) {
            return dirs[j].inode_off;
        }
    }
    return NOT_CONTAIN;
}

/**
   path can't be NULL
   open this file for aim process
   and return its file_table_entry
 */
static inode_t file_path(inode_t aim, char *path) {
    assert(path != NULL);
    char *save[MAX_DIR_DEPTH];
    int parts = split(path, '/', save);
    assert(parts <= MAX_DIR_DEPTH);
    int i;
    // assume it as a relative path
    inode_t node_off = aim;
    if (path[0] == '/') {// an absolute path
        // this assume root is always the first
        // inode of my file system
        node_off = inode_start;
    }
    for (i = 0; i < parts; i++) {
        if ((node_off = contain_file(node_off, save[i])) == 0) {// no such file or directory
            return INVALID_NODE_OFF;
        }
    }
    return node_off;
}

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

int list_dir(Msg *m) {
    char *name = (char *)m->dev_id;
    PCB *aim = (PCB *)m->buf;
    char *buf = (char *)get_pa(&aim->pdir, m->req_pid);
    assert(buf != NULL);
    // current working directory node_off
    inode_t node_off = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    if (name != NULL) {// using default file path
        node_off = file_path(node_off, name);
    }
    if (node_off == INVALID_NODE_OFF) {
        memcpy(buf, no_such, no_such_size);
    }
    iNode node;
    // get the node info of this file
    n_dev_read(now_disk, FM, &node, node_off, sizeof(iNode));
    // read the content of file by node info
    n_dev_read(now_disk, FM, buf, node.off, node.size);
    return SUCC;
}

int ch_dir(Msg *m) {
}