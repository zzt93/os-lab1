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
const int len_err = ARR_LEN(err);
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
static inode_t contain_file(inode_t node_off, char *name) {
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
inode_t file_path(inode_t cwd, const char * const name) {
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

/**
   check whether file exist by checking node offset with
   inode area's start
 */
static inline
int file_exist(inode_t off) {
    return off >= inode_start;
}

int default_file_block = 1;
/**
   return: the node offset of this empty file, if has
   any problem, return FAIL
   - set the inode's field of the newly created file
   and write back to now_disk
   - default only allocate a block for a newly-created file
   - write to father directory
 */
static
int make_empty_file(File_e type, char *name, PCB *aim,
    // the following one parameter is only useful for directory
    inode_t *dir_off) {
    iNode node;
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t dir;
    char *filename;
    // analyze file path
    // find the last '/' in the file path
    int last_slash = find_char(name, -1, '/');
    if (last_slash < 0) {// doesn't contain '/', i.e. just a file name
        dir = cwd;
        filename = name;
    } else {// directory/filename
        if (last_slash == 0) {// just like `/media`
            dir = inode_start;
        } else {
            // split name to directory and filename
            // by adding a '\0' between them
            name[last_slash] = '\0';
            // get the node offset of directory of this file
            dir = file_path(cwd, name);
        }
        filename = name + last_slash + 1;
    }
    if (!file_exist(dir)) {
        return FAIL;
    }
    // allocate new inode and default block
    inode_t new = inode_alloc();
    node.size = 0;
    node.dev_id = now_disk;
    int i;
    for (i = 0; i < default_file_block; i++) {
        node.index[i] = block_alloc();
    }
    for (i = default_file_block; i < FILE_LINK_NUM; i ++) {
        node.index[i] = 0;
    }
    node.link_count = 1;
    node.type = type;
    n_dev_write(now_disk, FM, &node, new, sizeof(iNode));

    // write to father directory's block
    Dir_entry dir_content;
    memcpy(dir_content.filename, filename, strlen(filename) + 1);
    dir_content.inode_off = new;
    write_block_file(dir, W_LAST_BYTE, (char *)&dir_content, sizeof(Dir_entry));

    // prepare returned pointer
    *dir_off = dir;
    return new;
}

static inline
int make_plain_file(char *name, PCB *aim) {
    inode_t i;
    return make_empty_file(PLAIN, name, aim, &i);
}

int create_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (invalid_filename(name)) {
        return FAIL;
    }
    return make_plain_file(name, aim) == FAIL ?
        FAIL : SUCC;
}

int make_dir(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (invalid_filename(name)) {
        return FAIL;
    }
    inode_t dir_off;
    // the node offset of new directory
    int new = make_empty_file(DIR, name, aim, &dir_off);
    if (!file_exist(new)) {
        return FAIL;
    }
    Dir_entry dir[2];
    // copy "."
    memcpy(dir[0].filename, current_dir, 2);
    dir[0].inode_off = new;
    // copy ".."
    memcpy(dir[1].filename, father_dir, 3);
    dir[1].inode_off = dir_off;
    write_block_file(new, W_LAST_BYTE, (char *)&dir, 2 * sizeof(Dir_entry));
    return new;
}

/**
   delete it in father's directory
   free inode, block in map
*/
int delete_a_file(inode_t father, inode_t this) {
    iNode this_node;
    n_dev_read(now_disk, FM,
        &this_node, this, sizeof this_node);
    if (this_node.type == DIR) {
        int num_files = this_node.size / sizeof(Dir_entry);
        assert(this_node.size % sizeof(Dir_entry) == 0);
        Dir_entry dirs[num_files];
        int i;
        for (i = 0; i < num_files; i++) {
            delete_a_file(this, dirs[i].inode_off);
        }
    } else {
        assert(this_node.type == PLAIN);
        iNode father_node;
        n_dev_read(now_disk, FM,
            &father_node, father, sizeof(iNode));
        // delete it in father's directory
        del_block_file_dir(&father_node, this);
        // free this file's block
        int index = this_node.size / block_size;
        int i;
        for (i = index; i >= 0; i--) {
            block_free(get_block(&this_node, index));
        }
        // free this file's inode
        inode_free(this);
    }
    return SUCC;
}

/**
   delete a regular file or
   a directory(which will delete the files it contains recursively)
*/
int delete_file(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (invalid_filename(name)) {
        return FAIL;
    }
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t dir;
    char *filename;
    // analyze file path
    // find the last '/' in the file path
    int last_slash = find_char(name, -1, '/');
    if (last_slash < 0) {// doesn't contain '/', i.e. just a file name
        dir = cwd;
        filename = name;
    } else {// directory/filename
        if (last_slash == 0) {// just like `/media`
            dir = inode_start;
        } else {
            // split name to directory and filename
            // by adding a '\0' between them
            name[last_slash] = '\0';
            // get the node offset of directory of this file
            dir = file_path(cwd, name);
        }
        filename = name + last_slash + 1;
    }
    if (!file_exist(dir)) {
        return FAIL;
    }
    inode_t new = contain_file(dir, filename);
    delete_a_file(dir, new);
    return SUCC;
}

int list_dir(Msg *m) {
    assert((char *)m->req_pid != NULL);
    PCB *aim = (PCB *)m->buf;
    char *buf = (char *)get_pa(&aim->pdir, m->req_pid);
    inode_t node_off;
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    char *name;
    if ((char *)m->dev_id == NULL) { // i.e. name is empty
        node_off = cwd;
    } else {
        name = (char *)get_pa(&aim->pdir, m->dev_id);
        // if not specify the list name,
        // using default file path -- current working directory node_off
        node_off = file_path(cwd, name);
    }
    if (node_off < inode_start) {
        set_error_msg(buf, node_off);
    }
    iNode node;
    // get the node info of this file
    n_dev_read(now_disk, FM, &node, node_off, sizeof(iNode));
    size_t read = 0;
    if (node.type == DIR) {
        // if name is NULL, it has to be go this branch
        // read the content of file by node info
        read = read_block_file(node_off, 0, buf, R_LAST_BYTE);
    } else {
        // not a directory, so just return it's parameter
        memcpy(buf, name, strlen(name) + 1);
        return FAIL;
    }
    return read / sizeof(Dir_entry);
}

int ch_dir(Msg *m) {
    PCB *aim = (PCB *)m->buf;
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    // if not specify the list name,
    // using default file path -- current working directory node_off
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t off = cwd;
    if (name != NULL) {
        off = file_path(cwd, name);
    }
    if (off < inode_start) {
        return FAIL;
    }
    iNode node;
    n_dev_read(now_disk, FM, &node, off, sizeof node);
    // set file table entry
    FTE *fte = add_fte(&node, off);
    set_cwd(aim, fte);
    return SUCC;
}

/* TODO need a write_to_file specify name && off_in_file??
   how to get offset???
size_t rw_prepare(Msg *m,
    size_t (*rw_block_file)(inode_t, uint32_t, char *buf, int len)) {
    PCB *aim = (PCB *)m->req_pid;
    char *buf = (char *)get_pa(&aim->pdir, (uint32_t)m->buf);
    char *name = (char *)get_pa(&aim->pdir, m->dev_id);
    if (invalid_filename(name)) {
        return FAIL;
    }
    // if not specify the list name,
    // using default file path -- current working directory node_off
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t nodeoff = file_path(cwd, name);
    if (nodeoff < inode_start) {
        return FAIL;
    }
    return rw_block_file(nodeoff, , buf, m->len);
}

size_t write_file(Msg *m) {
    return rw_prepare(m, write_block_file);
}

size_t n_read_file(Msg *m) {
    return rw_prepare(m, read_block_file);
}
*/