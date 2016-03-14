#include "assert.h"

int GCD(int a, int b) {
    int t = 0;
    while (b != 0) {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int LCM(int a, int b) {
    return a * b / GCD(a, b);
}

int pow(int a, int p) {
    assert(p >= 0);
    if (p == 0) {
        assert(a != 0);
        return 1;
    }
    int i;
    int res = 1;
    for (i = 0; i < p; i++) {
        res *= a;
    }
    return res;
}