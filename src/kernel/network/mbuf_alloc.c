//
// Created by zzt on 2/9/17.
//


#include <kernel/network/mbuf.h>
#include <kernel/network/mbuf_alloc.h>
#include "adt/bit_map.h"


static
MBuf bufs[MBUF_NUM];

BIT_MAP(MBUF_NUM);


MBuf *private_mbuf_alloc() {
    lock();
    int i = first_val(FREE);
    if (i == INVALID) {
        return NULL;
    }
    set_val(i, USED);
    unlock();
    return bufs + i;
}

void private_mbuf_free(MBuf *mBuf) {
    int i = mBuf - bufs;
    assert(i >= 0 && i < MBUF_NUM);
    lock();
    set_val(i, FREE);
    unlock();
}