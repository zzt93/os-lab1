#include "adt/bit_map.h"

#include "lib/random.h"

BIT_MAP(10)

void test_bit_map() {
    set_val(3, 1);
    assert(1 << 3 == free[0]);
}