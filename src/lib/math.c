
int pow(int a, int p) {
    //assert(p >= 0);
    if (p < 0) {
        return 0;
    }

    if (p == 0) {
        if (a == 0) {
            return 0;
        }
        return 1;
    }
    int i;
    int res = 1;
    for (i = 0; i < p; i++) {
        res *= a;
    }
    return res;
}

#define BCD8421_UNIT 4
#define BYTE 8
int BCD8421_to_int(unsigned int d) {
    int loop = BYTE * sizeof(d) / BCD8421_UNIT;
    int res = 0;
    unsigned int flag = 0xf;
    int digit = 1;
    int i;
    for (i = 0; i < loop; i++) {
        res += ((d & flag) * digit);
        digit *= 10;
        d >>= BCD8421_UNIT;
    }
    return res;
}