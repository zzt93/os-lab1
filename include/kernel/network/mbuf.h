//
// Created by zzt on 1/15/17.
//

#ifndef OS_LAB1_MBUF_H
#define OS_LAB1_MBUF_H

#include "types.h"
#include "interface.h"

typedef struct MB MBuf;

typedef enum {
    MT_CONTROL, // extra-data protocol message
    MT_DATA, // dynamic data allocation
    MT_FREE, // should be on free list
    MT_FTABLE, // fragment reassembly header
    MT_HEADER, // packet header
    MT_HTABLE, // IMP host tables
    MT_IFADDR, // interface address
    MT_OOBDATA, // expedited (out-of-band) data
    MT_PCB, // protocol control block
    MT_RIGHTS, // access rights
    MT_RTABLE, // routing tables
    MT_SONAME, // socket name
    MT_SOOPTS, // socket options
    MT_SOCKET, // socket structure
} EMBufType;

// default represent data flag
typedef enum {
    M_BCAST, //sent/received as link-level broadcast
    M_EOR, // end of record
    M_EXT, // cluster (external buffer) associated with this mbuf
    M_MCAST, //sent/received as link-level multicast
    M_PKTHDR, //first mubf that froms a packet (record)
} EMBufFlags;

#define M_CLUSTER_SIZE 2048
// max amount of data in mbuf with packet header
#define MHLEN 100
// max amount of data in normal mbuf
#define MLEN 108
// smallest amount of data to put into cluster
// MHLEN + MLEN: two mbuf, first have a header, second not
#define MIN_CLUSTER_SIZE (MHLEN + MLEN)
// size of each mbuf
#define MSIZE 128

typedef struct {
    MBuf *mh_next;
    MBuf *mh_nextpkt;

    void *mh_data;
    EMBufType mh_type;
    uint16_t mh_len;
    uint16_t mh_flags;
} MBHeader;

typedef struct {
    int len;
    NetworkInterface *rcvif;
} PacketHeader;

typedef struct {
    void *ext_buf;
    size_t ext_size;

    void (*ext_free)();
} MBExt;

struct MB {
    MBHeader m_hdr;
    union {
        struct {
            PacketHeader mh_pkthdr;
            union {
                MBExt ext;
                uint8_t mh_data[MHLEN];
            };
        };
        uint8_t data[MLEN];
    } m_dat;
};

//#define m_next MBHeader.mh_next
//#define m_len MBHeader.mh_len
//#define m_data MBHeader.mh_data
//#define m_type MBHeader.mh_type
//#define m_flags MBHeader.mh_flags
//#define m_nextpkt MBHeader.nextpkt
//
//#define m_pkthdr M_data.MH.MH_
//#define MBExt M_data.MH.MH_dat.ext
//#define m_pkidat M_data.MH.MH_dat.mh_data
//#define m_dat m_dat.M_databuf

MBuf *mbuf_get(int nowait, EMBufType type);
MBuf *mbuf_prepend(int len, int nowait);
//#define M_BUF_GET_HEADER(nowait, type)
//#define MH_ALIGN(m, len)

void mbuf_adj(MBuf *m, int len);
void mbuf_cat(MBuf *m, MBuf *n);


typedef struct {
    int m_clfree; // free clusters
    int m_clusters; // clusters obtained from page pool
    int m_drain;
    int m_mtypes;
} mbstat;


// mbuf to data
#define mtod(m, t) ((t)((m)->m_hdr.mh_data))
/**
 * data-to-mbuf
 * By knowing that MSIZE (128) is a power of 2,
 * and that mbufs are always
 * aligned by the kernel's memory allocator on MSIZE byte blocks of memory,
 * dtom just clears the appropriate low-order bits in its argument pointer
 * to find the beginning of the mbuf.
 *
 * it doesn't work if its argument points to a cluster, or within a cluster.
 * Since there is no pointer from the cluster back to the mbuf structure, dtom cannotbe used.
 */
#define dtom(x) ((MBuf)((int)(x) & ~(MSIZE-1)))

typedef enum {
    MB_PULL_FINE,
    MB_PULL_NO_MBUF,
    MB_PULL_LESS_DATA,
} EMBufPullErrorCode;

#endif //OS_LAB1_MBUF_H
