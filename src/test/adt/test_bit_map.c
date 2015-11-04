#include "adt/bit_map.h"
#include "assert.h"

#include "lib/random.h"

#define SIZE 40
BIT_MAP(SIZE)

void test_bit_map() {
    int i;
    // check initial state
    for (i = 0; i < SIZE / 32 + 1; i++) {
        assert(free[i] == 0);
    }
    // check value after single set
    for (i = 0; i < BITS; i++) {
        set_val(i, 1);
        assert(is_val(i, 1));
        assert(1 << i == free[0]);
        assert(first_val(1) == i);
        set_val(i, 0);
        assert(is_val(i, 0));
        assert(free[0] == 0);
        assert(first_val(0) == 0);
        assert(first_val(1) == INVALID);
    }
    for (i = BITS; i < SIZE; i++) {
        set_val(i, 1);
        assert(is_val(i, 1));
        assert(1 << (i-32) == free[1]);
        assert(first_val(1) == i);
        set_val(i, 0);
        assert(is_val(i, 0));
        assert(free[1] == 0);
        assert(first_val(0) == 0);
        assert(first_val(1) == INVALID);
    }

    // check state after multiple setting
    set_val(3, USED);
    set_val(4, USED);
    set_val(9, USED);
    set_val(20, USED);
    assert(free[0] ==
        (   (1 << 20) +
            (1 << 3) +
            (1 << 4) +
            (1 << 9) ));
}