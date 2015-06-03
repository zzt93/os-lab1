#include "kernel/MM.h"
#include "kernel/message.h"
#include "kernel/process.h"

#include "kernel/memory.h"

extern PCB* current;

/**
   对于进程需要用到的每一个虚拟地址va, 我们来模拟地址转换的过程,
   如果发现对应的页目录项无效, 那么就为这个页目录项分配一张页表,
   并填写页目录项;
   如果发现对应的页表项无效, 那么就为这个页表项分配一个物理页面,
   并填写页表项.
   FM需要把程序加载到这些物理页面中,
   使得将来用户进程运行的时候, 能够通过我们填写的页目录和页表, 找到一些物理页面, 同时这些物理页面中正好包含着程序的内容, 这样用户进程才能正确运行, 否则将会抛出异常.
   填写页目录项和页表项分别使用make_pde()函数和make_pte()函数, 它们都在src/kernel/memory/util.c中定义, 你可以在src/kernel/memory/kvm.c中参考内核启动分页机制的过程, 来看看如何使用它们.
 */
void alloc_page(Msg* m) {
    unsigned int off = (unsigned int)m->offset;
    assert(m->offset != 0);
    assert(m->len > 0);
    assert(m->buf & 0xfff == 0);
    
    // send back with the physical address
    m->buf = x;
    pid_t dest = m->src;
    m->dest = current->pid;
    send(dest, m);
}