#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/manager/f_dir.h"

#include "lib/string.h"

#include "error.h"

void test_list(char *name);
int set_name_msg(const char *name, int (*f)(Msg *));
static char first_doc[] = "test";
static char second_doc[] = "test2";
static char thi_doc[] = "test3";
static char fou_doc[] = "test4";
void test_create() {
    int res;
    /*
      res = set_name_msg(first_doc, make_dir);
      assert(res == SUCC);
      res = set_name_msg(first_doc, make_dir);
      assert(res == FILE_EXIST);
      res = set_name_msg(second_doc, make_dir);
      assert(res == SUCC);
      res = set_name_msg(thi_doc, make_dir);
      assert(res == SUCC);

      The upper code and the following code all cause
      exception #14, so
      1. it seems have something to do with number of file
      in a directory
      2. problem may in make_empty_file
      FIXED: for the bug in test_list
     */
    res = set_name_msg(first_doc, create_file);
    assert(res == SUCC);
    res = set_name_msg(first_doc, create_file);
    assert(res == FILE_EXIST);
    res = set_name_msg(second_doc, create_file);
    assert(res == SUCC);
    res = set_name_msg(thi_doc, create_file);
    assert(res == SUCC);
    res = set_name_msg(fou_doc, create_file);
    assert(res == SUCC);
}

/**
   @tested
 */
void test_delfile() {
    int res;
    res = set_name_msg(first_doc, delete_file);
    assert(res == SUCC);
    res = set_name_msg(first_doc, delete_file);
    assert(res == NO_SUCH);
    res = set_name_msg(second_doc, delete_file);
    assert(res == SUCC);
    res = set_name_msg(thi_doc, delete_file);
    assert(res == SUCC);
    res = set_name_msg(fou_doc, delete_file);
    assert(res == SUCC);
}

/**
   @tested: test whether system will crash when create and
   delete again and again, which may reveal some bugs
 */
void test_create_del() {
    int count = 100000;
    int i;
    for (i = 0; i < count; i++) {
        test_create();
        test_delfile();
    }
}

static int set_fd_msg(int fd, int (*f)(Msg *)) {
    Msg m;
    m.ret = FM_ERR;
    m.buf = current;
    m.dev_id = fd;
    int res = f(&m);
    test_list(NULL);
    if (m.ret == SUCC) {
        return res;
    }
    assert(res == INVALID_FD_I);
    return m.ret;
}

static int test_open(const char *name) {
    int fd;
    Msg m;
    m.ret = FM_ERR;
    m.req_pid = (int)current;
    m.buf = current;
    m.dev_id = (int)name;
    fd = open_file(&m);
    test_list(NULL);
    return fd;
}

static int test_close(int fd) {
    int res;
    res = set_fd_msg(fd, close_file);
    return res;
}


static int test_dup2(int old_fd, int new_fd) {
    int dup_fd;
    Msg m;
    m.ret = FM_ERR;
    m.i[2] = (int)current;
    m.i[0] = (int)old_fd;
    m.i[1] = (int)new_fd;
    dup_fd = dup2_file(&m);
    assert(m.ret == SUCC);
    test_list(NULL);

    return dup_fd;
}

#define LEN 64

static
void set_rw_msg(Msg *m, int fd, char *buffer) {
    m->ret = FM_ERR;
    m->i[0] = (pid_t)current;
    m->i[1] = fd;
    m->i[2] = (int)buffer;
    m->i[4] = LEN;
}

const uint8_t *const get_a_ram_file(int i, int *m_size);

static
void create_read_write(const char *name, int ram_id) {
    int res;
    // create file
    res = set_name_msg(name, create_file);
    assert(res == SUCC);
    // open
    int fd = test_open(name);
    // read from ram
    // write to disk
    Msg m;
    // get executable file
    int len = -1;
    char *buf = (char *)get_a_ram_file(ram_id, &len);
    set_rw_msg(&m, fd, buf);
    m.i[4] = len;
    write_file(&m);
    // close file
    res = test_close(fd);
    assert(res == fd);
}

/**
   read from ram then write to shell.out, exit.out
   TEST METHOD:
   - find the block offset in disk by print inode information
   - read out and show by `xxd -l file_size -c 4 -s start_offset -i harddisk.img`
   - compare result with shell.ar/exit.ar
   TESTED:
   - shell.ar is same with `xxd` result except the middle indirect index block
   - indirect index block's content is same with address of content block
   - indirect index block has 116 address which is the number of written size -- (116 + DATA_LINK_NUM) * 1kb
*/
const char bin[] = "/bin";
const char shell[] = "/bin/sh";
const char exit[] = "exit";
const char echo[] = "echo";
const char ls[] = "ls";
void read_ram_write_disk() {
    int res;
    // change directory to /bin/
    res = set_name_msg(bin, ch_dir);
    assert(res == SUCC);
    create_read_write(shell, 0);
    create_read_write(ls, 1);
    create_read_write(exit, 2);
    create_read_write(echo, 3);
}

static
void set_lseek_msg(Msg *m, int fd) {
    m->dev_id = fd;
    m->offset = 0;
    m->buf = current;
    m->len = SEEK_SET;
    int res = lseek_file(m);
    assert(res != -1);
    assert(m->ret == SUCC);
}

static
void test_write_read(int fd) {
    Msg m;
    char buffer[LEN] = {"I am a student"};
    char read_buffer[LEN];
    set_rw_msg(&m, fd, buffer);
    size_t w = write_file(&m);
    assert(w == LEN);
    assert(m.ret == SUCC);
    // test lseek by the way
    // first move cursor back
    set_lseek_msg(&m, fd);

    set_rw_msg(&m, fd, read_buffer);
    size_t r = n_read_file(&m);
    assert(r == w);
    assert(m.ret == SUCC);
    assert(strcmp(buffer, read_buffer) == 0);

    char read_buffer2[LEN];
    // now the cursor is at the end of file, so read should fail
    set_rw_msg(&m, fd, read_buffer2);
    m.len = -1;
    r = n_read_file(&m);
    assert(r == 0);

    set_lseek_msg(&m, fd);

    // test special parameter case -1
    set_rw_msg(&m, fd, read_buffer2);
    m.len = -1;
    r = n_read_file(&m);
    assert(r == w);
    assert(m.ret == SUCC);
    assert(strcmp(buffer, read_buffer) == 0);
}

void test_std_rw() {
    assert(current->fd_table[STDIN_FILENO].ft_entry != 0);
    assert(current->fd_table[STDOUT_FILENO].ft_entry != 0);
    assert(current->fd_table[STDERR_FILENO].ft_entry != 0);
    // write to stdout
    int fd = STDOUT_FILENO;
    Msg m;
    char buffer[] = {"I am a student\n"};
    //char read_buffer[LEN];
    set_rw_msg(&m, fd, buffer);
    int len = sizeof buffer / sizeof(char);
    m.len = len;
    size_t w = write_file(&m);
    assert(w == len);
    assert(m.ret == SUCC);
    // TODO add read from stdin then write
    fd = STDIN_FILENO;
    char read_buf[LEN];
    set_rw_msg(&m, fd, read_buf);
    size_t r = n_read_file(&m);
    assert(m.ret == SUCC);
    set_rw_msg(&m, STDERR_FILENO, read_buf);
    m.len = r;
    w = write_file(&m);
    assert(r == w);
}

/**
   @tested: open file, write file, read file, close_file
*/
void test_read_write() {
    // open a file and get fd
    int fd = test_open(first_doc);
    test_write_read(fd);
    int res = test_close(fd);
    assert(res == fd);
    test_std_rw();
}


/**
   @tested: open file, dup_file, dup2_file, close_file
*/
void test_open_close() {
    int dup_fd;
    int fd = test_open(first_doc);
    // fd should be 4
    // 0 -- stdin, 1 -- stdout, 2 -- stderr, 3 -- cwd
    assert(fd == CWD + 1);
    dup_fd = set_fd_msg(fd, dup_file);
    assert(dup_fd == fd + 1);
    int res;
    res = test_dup2(fd, dup_fd + 1);
    assert(res == dup_fd + 1);
    // do some operations: read and write to file
    // close the duplicated file descriptor
    res = test_close(dup_fd);
    assert(res == dup_fd);
    res = test_close(dup_fd + 1);
    assert(res == dup_fd + 1);
    res = test_close(fd);
    assert(res == fd);
}
