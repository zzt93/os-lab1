//
// Created by zzt on 1/16/17.
//


#include <kernel/network/mbuf.h>
#include "assert.h"
#include <adt/map.h>

MBuf *m_retry(int nowait, EMBufType type, int times);

void m_reclaim();

char assert_mbuf_size[sizeof(MBuf) % MSIZE == 0 ? 1 : -1];

void free_mbuf(MBuf *mBuf) {

}

static
MBuf *m_get(int nowait, EMBufType type, int retryCount) {
    MBuf *m = kmalloc(sizeof MBuf);
    if (m) {
        m->m_hdr.mh_type = type;
        // TODO MBUFLOCK
        m->m_hdr.mh_next = NULL;
        m->m_hdr.mh_nextpkt = NULL;
        m->m_hdr.mh_data = m->m_dat.data;
        m->m_hdr.mh_flags = (EMBufFlags) 0;
    } else {
        m = m_retry(nowait, type, retryCount);
    }
    return m;
}

MBuf *allocate_mbuf(int nowait, EMBufType type) {
    MBuf *m = m_get(nowait, type, 1);
    if (m == NULL) {
        panic("no enough memory to allocate mbuf");
    }
    return (m);
}

static
MBuf *m_retry(int nowait, EMBufType type, int times) {
    if (times == 0) {
        return NULL;
    }
    m_reclaim();
    return m_get(nowait, type, 0);
}

/**
 * each protocol can define a "drain" function
 * to be called by m_reclaim when the system gets low
 * on available memory
 */
static
void m_reclaim() {

}