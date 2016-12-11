#include "assert.h"
#include "lib/string.h"
#include "lib/random.h"

void test_string() {
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

void test_split() {

}