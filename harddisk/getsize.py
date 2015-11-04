
import os

__author__ = 'zzt'




class Img:
    SECTOR = 512

    # argument: ./disk.img
    def __init__(self, disk):
        self.disk = disk


    def cal_gap(self, size):
        gap = Img.SECTOR - size % Img.SECTOR
        if gap == Img.SECTOR:
            gap = 0

        return gap


    def get_size(self):
        disk_sz = os.path.getsize(self.disk)
        # although it is append at end, for they are all zero, so it's same
        super_gap = self.cal_gap(disk_sz)
        print(super_gap + disk_sz)
        print(super_gap, disk_sz)



if __name__ == '__main__':
    #print(os.getcwd())
    a = Img('../disk.img')
    a.get_size()
