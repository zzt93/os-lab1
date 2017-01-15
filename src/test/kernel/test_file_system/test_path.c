//
// Created by zzt on 11/18/16.
//


#include <kernel/manager/FM.h>
#include <kernel/kernel.h>
#include <lib/string.h>
#include <kernel/manager/f_dir.h>

int set_name_msg(const char *name, int (*f)(Msg *));

static char *absolute[] = {
        "/../.././...//a/",
        "/home/zzt/./he/.."
};

static char *result[] = {
        "/.../a",
        "/home/zzt"
};

static char *relative[] = {
        "../.././...//a/",
        "zzt/./he/..",
        "zzt/./he/a/.."
};

static char *result2[] = {
        "/.../a",
        "/home/zzt",
        "/home/zzt/he"
};


void test_simplify_path() {
    size_t s = sizeof(absolute) / sizeof(absolute[0]);
    for (int i = 0; i < s; ++i) {
        assert(strcmp(
                simplify_path(current->cwd_path, absolute[i]),
                result[i]) == 0);
    }

    int res = set_name_msg("/home", ch_dir);
    assert(res == SUCC);
    size_t s2 = sizeof(absolute) / sizeof(absolute[0]);
    for (int i = 0; i < s2; ++i) {
        assert(strcmp(
                simplify_path(current->cwd_path, relative[i]),
                result2[i]) == 0);
    }
}