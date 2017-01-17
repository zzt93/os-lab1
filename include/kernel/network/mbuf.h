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
} EBufType;

struct MB {
    MBuf *m_next;

    size_t m_len;
    void *m_data;
    EBufType m_type;

    union {
        typedef struct {
            size_t len;
            NetworkInterface *rcvif;
        } m_pkthdr;
        uint8_t data[sizeof(size_t) + sizeof(NetworkInterface *)];
    };
//    uint8_t more_data[];
};
#endif //OS_LAB1_MBUF_H
