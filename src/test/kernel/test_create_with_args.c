#include "assert.h"

void print_ch(int ch) {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("thread %d\n", ch);
        }
        x ++;
    }
}