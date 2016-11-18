//
// Created by zzt on 11/17/16.
//
#include <types.h>
#include <kernel/manager/f_dir.h>
#include "thread_annotations.h"
#include <lib/malloc.h>

static
const char *simplify_absolute_path(const char *path);

typedef struct {
    const char *s;
    int len;
} string;

static
void check_dir(const char *path, string *stack, int new_slash, int *sp, int *old_slash);


int is_absolute_path(const char *path) {
    if (path[0] == '/') {
        return true;
    }
    return false;
}

/**
 * Rules:
 * <li>remove redundant "/"</li>
 * <li>remove "." ".."</li>
 *
 * @param path
 * @return simplified path accroding to rules -- always be an **absolute path**
 */
const char *simplify_path(const char *cwd, const char *path)
NOT_THREAD_SAFE("assume only one thread invoke it") {

    if (!is_absolute_path(path)) { // relative path
        size_t len = strlen(cwd);
        void *tmp_path = kmalloc(len);
        memcpy(tmp_path, cwd, len);
        memcpy(tmp_path + len, path, +strlen(path) + 1);
        return simplify_absolute_path(tmp_path);
    }
    return simplify_absolute_path(path);
}


static char simple_path[MAX_PATH_LEN] = {0};

static
const char *simplify_absolute_path(const char *path) {
    assert(*path == '/');
    string stack[128];
    int sp = 0;

    int old_slash = 0;
    int i = 1; // skip first slash
    while (path[i] != '\0') {
        if (path[i] == '/') {
            check_dir(path, stack, i, &sp, &old_slash);
        }
        i++;
    }
    if (i != old_slash + 1) {
        check_dir(path, stack, i, &sp, &old_slash);
    }
    int len = 0;
    for (int j = 0; j < sp; ++j) {
        simple_path[len++] = '/';
        for (int l = 0; l < stack[j].len; ++l) {
            simple_path[len++] = stack[j].s[l];
        }
    }
    if (sp == 0) {
        simple_path[len++] = '/';
    }
    simple_path[len] = '\0';
    return simple_path;
}

static
void check_dir(const char *path, string *stack, int new_slash, int *sp, int *old_slash) {
    const char *dir = path + *old_slash + 1;
    int dir_len = new_slash - *old_slash - 1;
    assert(dir_len >= 0);
    if (dir_len == 0) {
    } else if (dir_len == 1 && strncmp(".", dir, dir_len) == 0) {
        // just skip
    } else if (dir_len == 2 && strncmp("..", dir, dir_len) == 0) {
        if ((*sp) != 0) {
            (*sp)--;
        }
    } else {
        string s = {dir, dir_len};
        stack[(*sp)++] = s;
    }
    (*old_slash) = new_slash;
};

