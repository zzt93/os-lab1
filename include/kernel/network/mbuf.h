//
// Created by zzt on 1/15/17.
//

#ifndef OS_LAB1_MBUF_H
#define OS_LAB1_MBUF_H

#include "types.h"
#include "interface.h"

typedef struct MB MBuf;

typedef enum {
    MT_SONAME, // socket name
    MT_DATA,
    MT_HEADER
} EBufType;

// default represent data flag
#define M_DEFAULT 0
#define M_PKTHDR 1
#define M_EXT (1 << 1)

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
    EBufType mh_type;
    int mh_flags;
} m_hdr;

typedef struct {
    int len;
    NetworkInterface *rcvif;
} pkthdr;

typedef struct {
    void *ext_buf;
    size_t ext_size;
    void (*ext_free)();
} m_ext;

struct MB {
    m_hdr m_hdr;
    union {
        struct {
            pkthdr MH_pkthdr;
            union {
                m_ext MH_ext;
                uint8_t mh_data[MHLEN];
            };
        };
        uint8_t data[MLEN];
    } M_dat;
};

#define m_next m_hdr.mh_next
#define m_len m_hdr.mh_len
#define m_data m_hdr.mh_data
#define m_type m_hdr.mh_type
#define m_flags m_hdr.mh_flags
#define m_nextpkt m_hdr.nextpkt

#define m_pkthdr M_data.MH.MH_
#define m_ext M_data.MH.MH_dat.MH_ext
#define m_pkidat M_data.MH.MH_dat.mh_data
#define m_dat M_dat.M_databuf


typedef struct {
    int m_clfree; // free clusters
    int m_clusters; // clusters obtained from page pool
    int m_drain;
    int m_mtypes;
} mbstat;
#endif //OS_LAB1_MBUF_H
