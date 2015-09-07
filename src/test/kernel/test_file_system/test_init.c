#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/manager/f_dir.h"

#include "lib/string.h"

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
        (int)entry, (int)NULL, current, INVALID_ID, SZ);
    m.ret = list_dir(&m);
    assert(m.ret != FAIL);
    size = m.ret;
    assert(strcmp(entry[0].filename, ".") == 0);
    assert(strcmp(entry[1].filename, "..") == 0);
    int i;
    for (i = 0; i < size; i++) {
        printk("%s, ", entry[i].filename);
    }

}

void test_mkdir() {
    Msg m;
    m.buf = current;
    char name[] = "first_dir";
    m.dev_id = (int)name;
    m.ret = make_dir(&m);
    assert(m.ret == SUCC);
    test_list(NULL);
}