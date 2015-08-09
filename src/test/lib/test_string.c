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

    char str[50] = "2 gcc -O2 -o test test.c";
    char *save[10] = {0};
    int res = split(str, ' ', save);
    assert(res == 6);
    int k = to_int(save[0]);
    assert(k == 2);
 }