#include "kernel/MM.h"
#include "kernel/message.h"
#include "kernel/process.h"

extern PCB* current;

/**
   每个进程只有一个页目录, 但页表和物理页面的数量是不确定的, 根据进程的需要进行分配.
   由于qemu的默认配置, Nanos的物理内存只有128MB.
   我们约定低16MB的空间分配给内核,
   剩下的112MB, 共28672个物理页面供用户进程使用. 如果使用0, 1, 2...对这些物理页面进行编号, 那么第n个物理页面的首地址就是16MB+(4n)KB.
 */