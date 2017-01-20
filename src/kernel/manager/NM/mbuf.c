//
// Created by zzt on 1/16/17.
//


#include <kernel/network/mbuf.h>
#include <const.h>

char assert_mbuf_size[sizeof(MBuf) % MSIZE == 0 ? 1 : -1];

void free_mbuf(MBuf *mBuf) {

}

MBuf *allocate_mbuf() {
    return NULL;
}