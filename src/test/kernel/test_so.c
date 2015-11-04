#include "common.h"

static void stackoverflow(int x) {
    if(x == 0) {
        printk("%d ",x);
    } else if(x > 0) {
        //printk("%d ", x);
        stackoverflow(x - 1);
    }
}

void keep_stackoverflow() {
    while(1) {
        stackoverflow(16384*1000);
    }
}