#include "kernel/MM.h"
//#include "kernel/message.h"
//#include "kernel/process.h"
#include "kernel/memory.h"
#include "kernel/semaphore.h"

#include "lib/string.h"

#define BITS 32
#define FREE_SIZE (USER_FREE_PAGES/BITS)


static Page* page_pa_start = (Page*)KERNEL_PA_END;
static uint32_t free[FREE_SIZE];
static uint32_t availabe = USER_FREE_PAGES;
/**
   每个进程只有一个页目录, 但页表和物理页面的数量是不确定的, 根据进程的需要进行分配.
   由于qemu的默认配置, Nanos的物理内存只有128MB.
   我们约定低16MB的physical address空间分配给内核,
   剩下的112MB, 共28672个物理页面供用户进程使用. 如果使用0, 1, 2...对这些物理页面进行编号, 那么第n个物理页面的首地址就是16MB+(4n)KB.
 */

/**
   set the i-th page to be valuse
   0 -- free
   1 -- used
 */
static void set_free(int i, int val) {
    assert(val == 0 || val == 1);
    assert(i >= 0 && i < USER_FREE_PAGES);
    int index = i / BITS;
    int j = i % BITS;
    int bit = val << j;
    assert(bit == (val << i));
    // if upper assert fail change all << i
    assert((free[index]&(1 << j)) != bit);
    // set that bit to val
    free[index] ^= (-val ^ free[index])
        & (1 << j);
}

static int is_free(int i) {
    assert(i >= 0 && i < USER_FREE_PAGES);
    int index = i / BITS;
    int j = i % BITS;
    lock();
    int res = free[index] >> j;
    unlock();
    return (res & 1) == 0;
}
/**
   return the physical address of a page
 */
void* alloc_page() {
    assert(0 < availabe);
    int i;
    for (i = 0; i < USER_FREE_PAGES; i++) {
        if (is_free(i)) {
            break;
        }
    }
    Page *page = page_pa_start + i;
    lock();
    set_free(i, 1);
    availabe--;
    unlock();
    memset(page, 0, PAGE_SIZE);
    return page;
}

void free_page(void *p) {
    assert(((uint32_t)p & 0xfff) == 0);
    Page* page = (Page*)p;
    int i = page - page_pa_start;
    lock();
    set_free(i, 0);
    availabe++;
    unlock();
}
