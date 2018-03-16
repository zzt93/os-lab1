#include "kernel/manager/MM.h"
//#include "kernel/message.h"
//#include "kernel/process.h"
#include "kernel/memory.h"
#include "kernel/semaphore.h"

#include "lib/string.h"
#include "adt/bit_map.h"

/**
   每个进程只有一个页目录, 但页表和物理页面的数量是不确定的, 根据进程的需要进行分配.
   由于qemu的默认配置, Nanos的物理内存只有128MB.
   我们约定低16MB的physical address空间分配给内核,
   剩下的112MB, 共28672个物理页面供用户进程使用. 如果使用0, 1, 2...对这些物理页面进行编号, 那么第n个物理页面的首地址就是16MB+(4n)KB.
 */
static Page *page_pa_start = (Page *) KERNEL_PA_END;
static uint32_t availabe = USER_FREE_PAGES;


BIT_MAP(USER_FREE_PAGES)

/**
   return the physical address of a page
 */
void *alloc_page() {
    assert(0 < availabe);
    int i = first_val(FREE);
    if (i == INVALID) {
        return NULL;
    }
    Page *page = page_pa_start + i;
    lock();
    set_val(i, USED);
    availabe--;
    unlock();
    memset(page, 0, PAGE_SIZE);
    return page;
}

/**
   p -- is the physical address
 */
void free_page(void *p) {
    assert(((uint32_t) p & 0xfff) == 0);
    Page *page = (Page *) p;
    int i = page - page_pa_start;
    lock();
    set_val(i, FREE);
    availabe++;
    unlock();
}
