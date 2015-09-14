#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/manager/f_dir.h"

#include "lib/string.h"

#include "error.h"

#define SZ 256
/**
   test the initial state of hard-disk
 */
void test_list(char *name) {
    Msg m;
    int size = SZ / sizeof(Dir_entry);
    Dir_entry entry[size];
    init_msg(&m,
        current->pid,
        FM_lsdir,
        (int)entry, (int)name, current, INVALID_ID, SZ);
    size = list_dir(&m);
    if (m.ret != SUCC) {
        printk("%s", err[m.ret]);
        return;
    }
    assert(strcmp(entry[0].filename, ".") == 0);
    assert(strcmp(entry[1].filename, "..") == 0);
    int i;

    for (i = 0; i < size; i++) {
        printk("%s, ", entry[i].filename);
    }

}

int set_name_msg(char *name, int (*f)(Msg *)) {
    Msg m;
    m.buf = current;
    m.dev_id = (int)name;
    f(&m);
    test_list(NULL);
    return m.ret;
}

static char name[] = "bin";
static char name2[] = "dev";
static char name3[] = "/home";
static char name4[] = "./media";
static char name5[] = "home/zzt/";
static char name6[] = "home/zzt/Desktop/";
static char name7[] = "home/zzt/Downloads/";
static char pic[] = "Picture";
static char doc[] = "Documents";
static char doc2[] = "Documents/os";

void test_mkdir() {
    int res;
    res = set_name_msg(name, make_dir);
    assert(res == SUCC);
    res = set_name_msg(name2, make_dir);
    assert(res == SUCC);
    // make a duplicate directory
    res = set_name_msg(name2, make_dir);
    assert(res == FILE_EXIST);
    // absolute path
    res = set_name_msg(name3, make_dir);
    assert(res == SUCC);
    // relative one
    res = set_name_msg(name4, make_dir);
    assert(res == SUCC);
    res = set_name_msg(name5, make_dir);
    assert(res == SUCC);
    res = set_name_msg(name6, make_dir);
    assert(res == SUCC);
    res = set_name_msg(name7, make_dir);
    assert(res == SUCC);

    test_list(name3);
    test_list(name5);
    test_list(NULL);
}

void test_deldir() {
    int res;
    res = set_name_msg(name, delete_file);
    assert(res == SUCC);
    res = set_name_msg(name, delete_file);
    assert(res == NO_SUCH);
    test_list(NULL);
}

void test_create() {
    int res;
    char first_doc[] = "test";
    res = set_name_msg(first_doc, create_file);
    assert(res == SUCC);
}

void test_delfile() {
}

void test_ch() {
    int res;
    res = set_name_msg(name5, ch_dir);
    assert(res == SUCC);
    test_list(NULL);

    res = set_name_msg(name4, make_dir);
    assert(res == SUCC);
    res = set_name_msg(pic, make_dir);
    assert(res == SUCC);
    res = set_name_msg(doc, make_dir);
    assert(res == SUCC);
    res = set_name_msg(doc2, make_dir);
    assert(res == SUCC);

    test_list(NULL);
}

static char repeat[] = "repeat";
void test_mk_del() {
    int res;
    int count = 100000;
    int i;
    for (i = 0; i < count; i++) {
        res = set_name_msg(repeat, make_dir);
        assert(res == SUCC);
        res = set_name_msg(repeat, delete_file);
        assert(res == SUCC);
    }
}