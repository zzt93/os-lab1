#include "adt/d_bit_map.h"
#include "assert.h"

#include "lib/random.h"

#define SIZE 40
D_BIT_MAP()

static uint8_t *array;
static void check_val(int j) {
    // check value after single set
    int i;
    for (i = j * BITS; i < (j + 1) * BITS && i < SIZE; i++) {
        set_val(i, 1);
        assert(is_val(i, 1));
        assert(1 << (i%BITS) == array[j]);
        assert(first_val(1) == i);
        set_val(i, 0);
        assert(is_val(i, 0));
        assert(array[j] == 0);
        assert(first_val(0) == 0);
        assert(first_val(1) == INVALID);
    }

}

void test_d_bit_map() {
    init_bitmap(SIZE);
    array = bits();
    int i;
    // check initial state
    int actual_size = SIZE % BITS == 0 ?
            SIZE / BITS : SIZE / BITS + 1;
    for (i = 0; i < actual_size; i++) {
        assert(array[i] == 0);
    }
    for (i = 0; i < actual_size; i++) {
        check_val(i);
    }

    // check state after multiple setting
    set_val(3, USED);
    set_val(4, USED);
    set_val(7, USED);
    set_val(2, USED);
    assert(array[0] ==
        (   (1 << 2) +
            (1 << 7) +
            (1 << 4) +
            (1 << 3) ));
}