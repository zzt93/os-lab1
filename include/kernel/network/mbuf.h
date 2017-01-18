//
// Created by zzt on 1/15/17.
//

#ifndef OS_LAB1_MBUF_H
#define OS_LAB1_MBUF_H

#include "types.h"
#include "interface.h"

typedef struct MB MBuf;

typedef enum  {
    MT_SONAME, // socket name
    MT_DATA,
    MT_HEADER
} EBufType;

// default represent data flag
#define M_DEFAULT 0
#define M_PKTHDR 1
#define M_EXT (1 << 1)

#define M_CLUSTER_SIZE 2048

struct MB {
    MBuf *m_next;
    MBuf *m_nextpkt;

    size_t m_len;
    void *m_data;
    EBufType m_type;
    int m_flags;

    union {
        typedef struct {
            size_t len;
            NetworkInterface *rcvif;
        } m_pkthdr;
        uint8_t data[sizeof(size_t) + sizeof(NetworkInterface *)];
    };
    union {
        typedef struct {
            void *ext_buf;
            size_t ext_size;
        } m_ext;
        uint8_t data2[sizeof(void *) + sizeof(size_t)];
    };
//    uint8_t more_data[];
};

typedef struct {
    int m_clfree; // free clusters
    int m_clusters; // clusters obtained from page pool

} mbstat;
#endif //OS_LAB1_MBUF_H
