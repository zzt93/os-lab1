#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/manager/f_dir.h"

#include "lib/string.h"

#define SZ 256
/**
   test the initial state of hard-disk
 */
void test_init() {
    Msg m;
    char buf[SZ];
    init_msg(&m,
        current->pid,
        FM_lsdir,
        (pid_t)buf, (int)NULL, current, INVALID_ID, SZ);
    int size = SZ / sizeof(Dir_entry);
    Dir_entry entry[size];
    assert(strcmp(entry[0].filename, ".") == 0);
    assert(strcmp(entry[1].filename, "..") == 0);
    int i;
    for (i = 0; i < size; i++) {
        printk("%s, ", entry[i].filename);
    }

}