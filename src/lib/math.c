
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
