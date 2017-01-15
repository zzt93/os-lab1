//
// Created by zzt on 1/15/17.
//

#ifndef OS_LAB1_MBUF_H
#define OS_LAB1_MBUF_H

#include "types.h"

typedef struct MB MBuf;

struct MB {
    MBuf *m_next;

    size_t m_len;
    void *m_data;

} ;
#endif //OS_LAB1_MBUF_H
