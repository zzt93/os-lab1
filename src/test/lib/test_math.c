#include "lib/math.h"
#include "assert.h"

void test_LCM() {
    int res = LCM(LCM(3, 4), 5);
    assert(res == 60);
    res = LCM(LCM(1, 2), 4);
    assert(res == 4);
    res = LCM(LCM(6, 3), 4);
    assert(res == 12);
}