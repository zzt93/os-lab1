//
// Created by zzt on 1/16/17.
//


#include <kernel/network/mbuf.h>
#include <kernel/network/mbuf_alloc.h>
#include "assert.h"

MBuf *m_retry(EMBufWait nowait, EMBufType type, int times);

void m_reclaim();

MBuf *m_get(EMBufWait nowait, EMBufType type, int retryCount);

char assert_mbuf_size[sizeof(MBuf) % MSIZE == 0 ? 1 : -1];
char assert_mbuf_header_size[sizeof(MBHeader) % (MSIZE - MLEN) == 0 ? 1 : -1];

/**
 * free single mBuf and return the `mBuf->next`
 * @param mBuf
 */
MBuf *mbuf_free(MBuf *mBuf) {

}

MBuf *mbuf_get(EMBufWait nowait, EMBufType type) {
    MBuf *m = m_get(nowait, type, 1);
    if (m == NULL) {
        panic("no enough memory to allocate mbuf");
    }
    // @see macro `dtom`
    assert(((uint32_t) m % MSIZE) == 0);
    return m;
}

MBuf *mbuf_dev_get(char *buf, int len, int off, NetworkInterface *ifp,
                   void (*copy)(const void *, void *, uint32_t)) {

}

/**
 * <li>rearranges the mbuf chain so that the first N bytes of data
 * are contiguous in the first mbuf on the chain.</li>
 * <li>IP reassembly and TCP reassembly</li>
 * @param mBuf
 * @param size
 * @return status code
 */
EMBufPullErrorCode m_pullup(MBuf *mBuf, uint16_t size) {

    return MB_PULL_FINE;
}

/*
 * -------------------------- static method ------------------------------
 */

static
MBuf *m_get(EMBufWait nowait, EMBufType type, int retryCount) {
    MBuf *m = private_mbuf_alloc();
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

static
MBuf *m_retry(EMBufWait nowait, EMBufType type, int times) {
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