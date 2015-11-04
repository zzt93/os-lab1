#include "lib/random.h"
#include "assert.h"

void test_random() {
    printk("%d %d ", sizeof(long), sizeof(long long int));
    int i;
    // check range
    for (i = 1; i < 1000; i++) {
        assert(next_int(i) < i);
    }
    // check uniformity
    int res;
    int c0 = 0;
    int c1 = 0;
    for (i = 0; i < 100000; i++) {
        // res is 0 or 1
        res = next_int(2);
        (res == 0) ? c0++ : c1++;
    }
    printk("%d %d ", c0, c1);
    c0 = 0;
    c1 = 0;
    int c2 = 0;
    assert(c0 - c1 < 100 && c0 - c1 > -100);
    for (i = 0; i < 150000; i++) {
        res = next_int(3);
        if (res == 0) {
            c0++;
        } else if (res == 1) {
            c1++;
        } else if (res == 2) {
            c2++;
        } else {
            assert(0);
        }
    }
    printk("%d %d %d ", c0, c1, c2);
    assert(c0 - c1 < 200 && c0 - c1 > -200);
    assert(c0 - c2 < 200 && c0 - c2 > -200);
}