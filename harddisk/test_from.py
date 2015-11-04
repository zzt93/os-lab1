from _ctypes import sizeof
from learn_class.os.makeimg import DirEntry

__author__ = 'zzt'

if __name__ == '__main__':
    b = bytearray(sizeof(DirEntry))
    print(b)
    entry = DirEntry.from_buffer(b)
    entry.inode_off = 1
    entry.filename = b'abc'
    print(b)
