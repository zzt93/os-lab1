#include "drivers/time.h"

/**
   Test method:
   b test_nb_timer.c:10, 14;
 */
void test_nb_timer() {
    int i;
    for (i = 1; i < 5; i++) {
        kstart_non_block_timer(i);
        while (!knon_block_timer_finished()) {
            printk(".");
        }
        printk(RED"%s execute #%d second(s)\n"RESET, "nb timer", i);
    }
}