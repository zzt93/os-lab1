#include "kernel/message.h"
#include "drivers/hal.h"
#include "kernel/process.h"
#include "kernel/manager/MM_util.h"
#include "kernel/manager/FM.h"

#include "kernel/manager/inode.h"
#include "kernel/manager/block.h"
#include "kernel/manager/f_dir.h"

#include "lib/string.h"

char test_sizeof_Dir_entry[sizeof(Dir_entry) % 32 == 0 ? 1 : -1];

const char *const err[] = {
    "No such file or directory",
    "Not a directory",
};
int err_size[ARR_LEN(err)];

static
void set_error_msg(char *buf, int i) {
    memcpy(buf, err[i], err_size[i]);
}

const char *const current_dir = ".";
const char *const father_dir = "..";
const char *const default_cwd_name = "/";

/**
   test whether a directory contain a file by check the
   name list it store
   @return:
   0 -- not contain
   node_offset of the file which called `name`
 */
static int contain_file(inode_t node_off, char *name) {
    iNode node;
    n_dev_read(now_disk, FM, &node, node_off, sizeof node);
    if (node.type != DIR) {
        return NOT_DIR;
    }
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
    return NO_SUCH;
}

/**
   Analyze the file name with path
   and return node offset of that file
   -- path can't be NULL
 */
static inode_t file_path(inode_t cwd, char *name) {
    assert(name != NULL);
    // assume it as a relative path
    // i.e., set node_off as cwd's node
    inode_t node_off = cwd;
    if (name[0] == '/') {// an absolute path
        // set node_off as the root
        // this assume root is always the first
        // inode of my file system
        node_off = inode_start;
    }
    char *save[MAX_DIR_DEPTH];
    int len = strlen(name) + 1;
    char path[len];
    memcpy(path, name, len);
    // TODO split "//afd/asd"
    int parts = split(path, '/', save);
    assert(parts <= MAX_DIR_DEPTH);
    int i;
    for (i = 0; i < parts; i++) {
        node_off = contain_file(node_off, save[i]);
        if (node_off == NO_SUCH) {// no such file or directory
            return NO_SUCH;
        } else if (node_off == NOT_DIR) {
            return NOT_DIR;
        }
    }
    return node_off;
}

static inline
int file_exist(inode_t off) {
    return off >= inode_start;
}

int create_file(Msg *m) {
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (name == NULL) {
        return FAIL;
    }
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t dir;
    char *filename;
    int last_slash = find(name, -1, '/');
    if (last_slash < 0) {// doesn't contain '/'
        dir = cwd;
        filename = name;
    } else {
        filename = name + last_slash + 1;
        name[last_slash] = '\0';
        dir = file_path(cwd, name);
    }
    if (!file_exist(dir)) {
        return FAIL;
    }

    // whether it is already exist
    return SUCC;
}

int make_dir(Msg *m) {
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (name == NULL) {
        return FAIL;
    }
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t off = file_path(cwd, name);
    if (file_exist(off)) {
        return FAIL;
    }


    return SUCC;
}

/**
   create a regular file or a directory
*/
int delete_file(Msg *m) {
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (name == NULL) {
        return FAIL;
    }
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t off = file_path(cwd, name);
    // whether it is exist
    if (!file_exist(off)) {
        return FAIL;
    }

    return SUCC;
}

int list_dir(Msg *m) {
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    PCB *aim = (PCB *)m->buf;
    char *buf = (char *)get_pa(&aim->pdir, m->req_pid);
    assert(buf != NULL);
    // if not specify the list name,
    // using default file path -- current working directory node_off
    inode_t node_off = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    if (name != NULL) {
        node_off = file_path(node_off, name);
    }
    if (node_off < inode_start) {
        set_error_msg(buf, node_off);
    }
    iNode node;
    // get the node info of this file
    n_dev_read(now_disk, FM, &node, node_off, sizeof(iNode));
    if (node.type == DIR) {
        // read the content of file by node info
        read_file(buf, &node, 0, node.size);
    } else {
        // not a directory, so just return it's parameter
        memcpy(buf, name, strlen(name) + 1);
    }
    return SUCC;
}

int ch_dir(Msg *m) {
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (name == NULL) {
        name = default_cwd_name;
    }
    PCB *aim = (PCB *)m->buf;
    // if not specify the list name,
    // using default file path -- current working directory node_off
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t off = file_path(cwd, name);
    if (off < inode_start) {
        return FAIL;
    }
    // set file table entry
    iNode node;
    n_dev_read(now_disk, FM, &node, off, sizeof node);
    FTE *fte = add_fte(&node, off);
    set_cwd(aim, fte);
}