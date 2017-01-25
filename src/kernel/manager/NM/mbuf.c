//
// Created by zzt on 1/16/17.
//


#include <kernel/network/mbuf.h>
#include "assert.h"
#include <adt/map.h>

char assert_mbuf_size[sizeof(MBuf) % MSIZE == 0 ? 1 : -1];

void free_mbuf(MBuf *mBuf) {

}

static
MBuf * m_get(int nowait, int type) {
    MBuf *m = kmalloc(sizeof MBuf);
    if (m) {
        m->m_hdr.mh_type = (EBufType) type;
        // TODO MBFLOCK
        m->m_hdr.mh_next = NULL;
        m->m_hdr.mh_nextpkt = NULL;
        m->m_hdr.mh_data = m->m_dat.data;
        m->m_hdr.mh_flags = (EMbufFlags) 0;
    } else {

    }
    return m;
}

MBuf *allocate_mbuf(int nowait, int type) {
    MBuf *m = m_get(nowait, type);
    if (m == NULL) {
        panic("no enough memory to allocate mbuf");
    }
    return (m);
}