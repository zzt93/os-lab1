#include "assert.h"
#include "lib/string.h"
#include "lib/random.h"
#include "lib/malloc.h"


static void test_mis();

static void test_split();

static void test_trim();

void test_string() {
    test_split();
    test_trim();
    test_mis();
}

void test_mis() {
    srand(123);
    int i = next_int(12345);
    char buf[10];
    itoa_s(i, buf, 10);
    printk("%s ", buf);
    int j = to_int(buf);
    assert(i == j);
    printk("%d ", j);

    // test split
    char str[50] = "2 gcc -O2 -o test test.c";
    char *save[10] = {0};
    int res = split(str, ' ', save);
    assert(res == 6);
    int k = to_int(save[0]);
    assert(k == 2);

    // test find_char
    char *path[] = {
            "/a/b/c/d",
            "./a/b/c/d",
            "../a/b/c/d",
            "aaa/b/c/d"
    };
    int index[ARR_LEN(path)];
    int in;
    for (in = 0; in < ARR_LEN(path); in++) {
        index[in] = find_char(path[in], -1, '/');
    }
    for (in = 0; in < ARR_LEN(path); in++) {
        assert(index[in] == strlen(path[in]) - 2);
    }

    for (in = 0; in < ARR_LEN(path); in++) {
        index[in] = find_char(path[in], 1, '/');
    }
    for (in = 0; in < ARR_LEN(path); in++) {
        assert(index[in] == in);
    }
}

static
void test_split() {
    static char *paths[] = {
            "b",
            "as/b",
            "sdf//b",
            "zzt/./he/b",
            "zzt/./he/a/b",
            "/zzt/./he/../b",
            "../.././...//a/b",
            "/../.././...//a/b",
    };

    char *save[64] = {0};
    for (int i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        int r = split(paths[i], '/', save);
        assert(r == i + 1);
        assert(save[i] != NULL);
        assert(strcmp(save[i], "b") == 0);
    }
}

static
void test_trim() {
    static char *strings[] = {
            "", " ", " a", "a ", " a ", "  a", "a  ", "  a ", " a  ", "  a  "
    };
    int size = 64;
    char *array = kmalloc(size);
    for (int i = 0; i < sizeof(strings) / sizeof(strings[0]); ++i) {
        memcpy(array, strings[i], strlen(strings[i]) + 1);
        trim(array, ' ');
        if (i >= 2) {
            assert(strcmp(array, "a") == 0);
        } else {
            assert(array[0] == '\0');
        }
    }
}