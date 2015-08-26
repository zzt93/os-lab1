__author__ = 'zzt'


class BitMap:
    USED = 1
    FREE = 0
    BITS = 8
    '''
    bits is start from l[i], having size bits all
    l is a bytearray, ie each element has 8 bits
    '''

    def __init__(self, l, i, sz):
        self.l = l
        self.start = i
        self.size = sz

    def first_val(self, value):
        for x in range(self.size):
            if self.is_val(x, value):
                return x

    def is_val(self, x, value):
        assert 0 <= x < self.size
        index = x // BitMap.BITS
        j = x % BitMap.BITS
        res = self.l[index + self.start] >> j
        return (res & 1) == value

    def set_val(self, x, value):
        assert 0 <= x < self.size
        index = x // BitMap.BITS
        j = x % BitMap.BITS
        bit = value << j
        assert (self.l[index + self.start] & (1 << j)) != bit
        self.l[index + self.start] ^= ((-value ^ self.l[index + self.start]) & (1 << j))

    def apply_bit(self):
        x = self.first_val(BitMap.FREE)
        self.set_val(x, BitMap.USED)
        return x
