#include <kernel/kernel.h>
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


static
void set_error_msg(char *buf, int i) {
    assert(i < 0);
    memcpy(buf, err[-i], err_size[-i]);
}

const char *const current_dir = ".";
const char *const father_dir = "..";
const char *const default_cwd_name = "/";

/**
   test whether a directory contain a file by check the
   name list it store
   @return:
   node_offset of the file which called `name`
   NO_SUCH -- no name
   NOT_DIR -- dir_node_off is not directory
 */
static inode_t contain_file(inode_t dir_node_off, const char *name) {
    if (filename_too_long(name)) {
        return NO_SUCH;
    }
    iNode node;
    n_dev_read(now_disk, FM, &node, dir_node_off, sizeof node);
    if (node.type != NODE_DIR) {
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
        i++;
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
    Analyze the simplified file name with path
    and return node offset of that file
    -- path can't be NULL

    @see simplify_path

    TODO this may return NO_SUCH which may in range of [inode_start,
    inode_start + inode_area_size]
 */
inode_t file_nodeoff(inode_t cwd, const char *const name) {
    assert(name != NULL);
    // assume it as a absolute  path
    // set node_off as the root
    // this assume root is always the first
    // inode of my file system
    inode_t node_off = inode_start;
    if (name[0] != '/') {// an relative path -- impossible after simplify path
        assert(false);
        // i.e., set node_off as cwd's node
        node_off = cwd;
    }
    char *save[MAX_DIR_DEPTH];
    size_t len = strlen(name) + 1;
    assert(len >= 2);
    char path[len];
    memcpy(path, name, len);

    assert(path[0] == '/');
    // to skip the first '/' so make split easier
    int parts = split(path + 1, '/', save);
    assert(parts <= MAX_DIR_DEPTH);
    int i;
    for (i = 0; i < parts; i++) {
        node_off = contain_file(node_off, save[i]);
        if (node_off == NO_SUCH) {// save[i] not exist
            return NO_SUCH;
        } else if (node_off == NOT_DIR) {// node_off exist but not a directory
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
    return off >= inode_start && off < inode_start + inode_area_size;
}

const int default_file_block = 1;

/**
   return: the node offset of this empty file, if has
   any problem, return FAIL
   - set the inode's field of the newly created file
   and write back to now_disk
   - default only allocate a block for a newly-created file
   - write to father directory

   - FIXED -- add `const char *` and `memcpy`: this function change the content of name, may be changed later
 */
static
inode_t make_empty_file(ENodeType type, const char *fname, PCB *aim,
        // the following one parameter is only useful for directory
                        inode_t *dir_off) {
    inode_t cwd = ((FTE *) aim->fd_table[CWD].ft_entry)->node_off;
    inode_t dir;
    size_t filenamelen = strlen(fname) + 1;
    char name[filenamelen];
    memcpy(name, fname, filenamelen);
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
            if (name[last_slash + 1] == '\0') {
                // e.g. make a directory: /media/
                // TODO use simplified path invalidate this occasion
                assert(false);
                if (type == NODE_PLAIN) {
                    return FAIL;
                }
                // re-find a meaningful '/';
                last_slash = find_char(name, -1, '/');
                name[last_slash] = '\0';
            }

            // get the node offset of directory which this file resides
            dir = file_nodeoff(cwd, name);
        }
        filename = name + last_slash + 1;
    }
    // directory not exist or file is already exist
    if (!file_exist(dir)) {
        return NO_SUCH;
    }
    if (filename_too_long(filename)) {
        return INVALID_FILENAME;
    }
    if (file_exist(contain_file(dir, filename))) {
        return FILE_EXIST;
    }
    // allocate new inode and default block
    inode_t new = inode_alloc();
    if (new == INVALID_ALLOC) {
        return NO_MORE_DISK;
    }

    iNode node;
    node.size = 0;
    node.dev_id = now_disk;
    int i;
    for (i = 0; i < default_file_block; i++) {
        node.index[i] = block_alloc();
        if (node.index[i] == INVALID_ALLOC) {
            // for make a new file failed, free it's resource
            inode_free(new);
            return NO_MORE_DISK;
        }
    }
    for (i = default_file_block; i < FILE_LINK_NUM; i++) {
        node.index[i] = 0;
    }
    node.link_count = 1;
    node.type = type;
    n_dev_write(now_disk, FM, &node, new, sizeof(iNode));

    // write to father directory's block
    Dir_entry dir_content;
    memcpy(dir_content.filename, filename, strlen(filename) + 1);
    dir_content.inode_off = new;
    int len = write_block_file(now_disk, dir, W_LAST_BYTE, (char *) &dir_content, sizeof(Dir_entry));
    if (len != sizeof(Dir_entry)) {
        return NO_MORE_DISK;
    }

    // prepare returned pointer
    *dir_off = dir;
    return new;
}

static inline
inode_t make_plain_file(const char *name, PCB *aim) {
    inode_t ignore;
    return make_empty_file(NODE_PLAIN, name, aim, &ignore);
}

int create_file(Msg *m) {
    PCB *aim = (PCB *) m->buf;

    const char *name = simplify_path(aim->cwd_path,
                                     (const char *) get_pa(&aim->pdir, m->dev_id));
    if (null_filename(name)) {
        return INVALID_FILENAME;
    }
    inode_t res = make_plain_file(name, aim);
    m->ret = res;
    if (file_exist(res)) {
        m->ret = SUCC;
    }
    return res;
}

int make_dir(Msg *m) {
    PCB *aim = (PCB *) m->buf;

    const char *name = simplify_path(aim->cwd_path,
                                     (const char *) get_pa(&aim->pdir, m->dev_id));
    if (name == NULL) {
        return INVALID_FILENAME;
    }
    inode_t dir_off;
    // the node offset of new directory
    inode_t new = make_empty_file(NODE_DIR, name, aim, &dir_off);
    if (!file_exist(new)) {
        m->ret = new;
        return new;
    }
    m->ret = SUCC;

    Dir_entry dir[2];
    // copy "." to new directory
    memcpy(dir[0].filename, current_dir, 2);
    dir[0].inode_off = new;
    // copy ".."
    memcpy(dir[1].filename, father_dir, 3);
    dir[1].inode_off = dir_off;
    write_block_file(now_disk, new, W_LAST_BYTE, (char *) &dir, 2 * sizeof(Dir_entry));

    return new;
}

/**
   delete it in father's directory
   free inode, block in map
*/
static
int delete_a_file(inode_t father, inode_t this) {
    iNode this_node;
    n_dev_read(now_disk, FM,
               &this_node, this, sizeof this_node);
    if (this_node.type == NODE_DIR) {
        int num_files = this_node.size / sizeof(Dir_entry);
        assert(this_node.size % sizeof(Dir_entry) == 0);
        Dir_entry dirs[num_files];
        // TESTED test add now_disk
        read_block_file(now_disk, this, 0, (char *) dirs, R_LAST_BYTE);
        assert(strcmp(dirs[0].filename, current_dir) == 0);
        assert(strcmp(dirs[1].filename, father_dir) == 0);
        int i;
        // skip current_dir and father_dir for it
        // doesn't need to
        // delete its child file
        for (i = 2; i < num_files; i++) {
            delete_a_file(this, dirs[i].inode_off);
        }
    }
    // delete it in father's directory
    del_block_file_dir(father, this);
    // free this file's block
    int index = this_node.size / block_size;
    int i;
    for (i = index; i >= 0; i--) {
        block_free(get_block(&this_node, index));
    }
    // free this file's inode
    inode_free(this);
    return SUCC;
}

/**
   delete a regular file or
   a directory(which will delete the files it contains recursively)

   - this function change the content of name, may be changed later
*/
int delete_file(Msg *m) {
    PCB *pcb = (PCB *) m->buf;

    const char *fname = simplify_path(pcb->cwd_path,
                                      (const char *) get_pa(&pcb->pdir, m->dev_id));
    if (null_filename(fname)) {
        return FM_ERR;
    }
    inode_t cwd = ((FTE *) pcb->fd_table[CWD].ft_entry)->node_off;
    inode_t dir;
    size_t filenamelen = strlen(fname) + 1;
    char name[filenamelen];
    memcpy(name, fname, filenamelen);
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
            if (name[last_slash + 1] == '\0') {
                // e.g. /media/
                // re-find a meaningful '/';
                // TODO simplify path invalidate this case
                assert(false);
                last_slash = find_char(name, -1, '/');
                name[last_slash] = '\0';
            }

            // get the node offset of directory of this file
            dir = file_nodeoff(cwd, name);
        }
        filename = name + last_slash + 1;
    }
    if (!file_exist(dir)) {
        m->ret = NO_SUCH;
        return NO_SUCH;
    }
    inode_t aim = contain_file(dir, filename);
    if (aim == NO_SUCH || aim == NOT_DIR) {
        m->ret = NO_SUCH;
        return NO_SUCH;
    }
    m->ret = delete_a_file(dir, aim);
    return aim;
}

/**
   return: how many directory entry is read
 */
int list_dir(Msg *m) {
    assert((char *) m->req_pid != NULL);
    PCB *aim = (PCB *) m->buf;
    char *buf = (char *) get_pa(&aim->pdir, m->req_pid);
    inode_t node_off;
    inode_t cwd = ((FTE *) aim->fd_table[CWD].ft_entry)->node_off;
    const char *name;
    if ((char *) m->dev_id == NULL) {
        node_off = cwd;
        name = NULL;
    } else {
        name = simplify_path(aim->cwd_path,
                             (const char *) get_pa(&aim->pdir, m->dev_id));
        if (str_empty(name)) { // i.e. name is empty
            node_off = cwd;
        } else {
            // if not specify the list name,
            // using default file path -- current working directory node_off
            node_off = file_nodeoff(cwd, name);
        }
    }
    if (!file_exist(node_off)) {
        m->ret = NO_SUCH;
        set_error_msg(buf, node_off);
        return NO_SUCH;
    }
    iNode node;
    // get the node info of this file
    n_dev_read(now_disk, FM, &node, node_off, sizeof(iNode));
    if (node.size > m->len) {
        m->ret = BUF_OF;
        return BUF_OF;
    }
    size_t read = 0;
    m->ret = SUCC;
    if (node.type == NODE_DIR) {
        // if name is NULL, it has to be go this branch
        // read the content of file by node info
        read = read_block_file(now_disk, node_off, 0, buf, R_LAST_BYTE);
        return read / sizeof(Dir_entry);
    } else {
        // not a directory, so just return it's parameter
        assert(name != NULL);
        memcpy(buf, name, strlen(name) + 1);
        return 1;
    }
}


int ch_dir(Msg *m) {
    PCB *aim = (PCB *) m->buf;
    const char *name = simplify_path(aim->cwd_path, (const char *) get_pa(&aim->pdir, m->dev_id));
    // if not specify the list name,
    // using default file path -- current working directory node_off
    inode_t cwd = ((FTE *) aim->fd_table[CWD].ft_entry)->node_off;
    inode_t off = cwd;
    assert(off >= inode_start);
    if (name != NULL) {
        off = file_nodeoff(cwd, name);
        if (!file_exist(off)) {
            m->ret = NO_SUCH;
            return FAIL;
        }
        // update cwd_path
        if (is_absolute_path(name)) {
            free_cwd_path(aim);
            set_cwd_path(aim, name);
        } else {
            assert(false);
            append_cwd_path(aim, name);
        }
    }
    iNode node;
    n_dev_read(now_disk, FM, &node, off, sizeof node);
    // set file table entry
    FTE *fte = add_fte(&node, off);
    set_cwd(aim, fte);

    m->ret = SUCC;
    return off;
}

/* need a write_to_file specify name && off_in_file??
   1. how to get offset???
   2. too much arguments: char *content_buf, int write_len,
   char *file_name, int off_in_file, PCB *aim,

   maybe a version of writing file to create a file
   i.e. create a plain file, open file, no offset,
   write content to it
   and return fd

size_t rw_prepare(Msg *m,
    size_t (*rw_block_file)(inode_t, uint32_t, char *buf, int len)) {
    PCB *aim = (PCB *)m->req_pid;
    char *buf = (char *)get_pa(&aim->pdir, (uint32_t)m->buf);
    const char *name = (const char *)get_pa(&aim->pdir, m->dev_id);
    if (null_filename(name)) {
        return FAIL;
    }
    // if not specify the list name,
    // using default file path -- current working directory node_off
    inode_t cwd = ((FTE *)aim->fd_table[CWD].ft_entry)->node_off;
    inode_t nodeoff = file_nodeoff(cwd, name);
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