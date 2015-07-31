#include "kernel/semaphore.h"

/**
   Using the linear congruential generator to produce random
   number
   The generator is defined by the recurrence relation:

   X_{n+1} = ( a X_n + c ) mod m

   where X is the sequence of pseudorandom values, and

   m, 0 < m – the "modulus"
   a, 0 < a < m – the "multiplier"
   c, 0 < c < m – the "increment"
   X_0, 0 < X_0 < m – the "seed" or "start value"

   The choice of a, c is very important for the quality
   of random number and I choose the number of java.
   @see Random.java
 */

static long long int rseed;
static const long long int a = 0x5DEECE66D;
static const long int c = 0xb;
static const long long int mask = (1ll << 48) - 1;
static const double DOUBLE_UNIT = 1.0 / (1ll << 53);

static int next(int bits) {
    long long int old, next;
    lock();
    do {
        old = rseed;
        next = (a * old + c) & mask;
    } while (old == next);
    rseed = next;
    unlock();
    /*
      对于x86平台的gcc编译器，最高位移入1，也就是仍保持负数的符号位，
      这种处理方式对负数仍然保持了“右移1位相当于除以2”的性质。
     */
    return (int)(next >> (48 - bits));
}

double next_double() {
    return (((long long int)next(26) << 27) + next(27)) * DOUBLE_UNIT;
}

/**
   return random int between [0, round)
 */
int next_int(int bound) {
    if (bound <= 0) {
        assert(0);
    }
    if (!(bound & (bound - 1))) {  // i.e., bound is a power of 2
        return (int)((bound * (long long int)next(31)) >> 31);
    }
    /*'
      if bound is not the power of 2, just using modulus may
      introduce large bias.
     */
    int bits, val;
    do {
        // always be positive number
        bits = next(31);
        val = bits % bound;
    } while (bits - val + (bound-1) < 0);
    return val;
}

void srand(unsigned int seed) {
    assert(sizeof(long long int) == 8);
    lock();
    rseed = seed;
    unlock();
}
