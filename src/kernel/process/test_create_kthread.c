#include "common.h"

void A () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {printk("a");}
        x ++;
    }
}

void B () {
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {printk("b");}
        x ++;
    }
}