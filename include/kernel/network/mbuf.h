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
// smallest amount of data to put into cluster
#define M_IN_CL_SIZE 208
// max amount of data in normal mbuf
#define MLEN 108
// size of each mbuf
#define MSIZE 128

typedef struct {
    MBuf *mh_next;
    MBuf *mh_nextpkt;

    size_t mh_len;
    void *mh_data;
    EMBufType mh_type;
    EMBufFlags mh_flags;
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

MBuf *allocate_mbuf(int nowait, EMBufType type);

#include "malloc.h"



typedef struct {
    int m_clfree; // free clusters
    int m_clusters; // clusters obtained from page pool
    int m_drain;
    int m_mtypes;
} mbstat;
#endif //OS_LAB1_MBUF_H
