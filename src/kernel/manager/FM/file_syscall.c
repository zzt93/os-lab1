#include "kernel/message.h"

#include "kernel/manager/inode.h"
#include "kernel/manager/block.h"
#include "kernel/manager/f_dir.h"

int create_file(Msg *m) {
    // whether it is already exist
    return SUCC;
}

int make_dir(Msg *m) {
    return SUCC;
}

int delete_file(Msg *m) {
    // whether it is exist
    return SUCC;
}

int delete_dir(Msg *m) {
    return SUCC;
}

int list_dir(Msg *m) {
    return SUCC;
}