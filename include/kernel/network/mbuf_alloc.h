//
// Created by zzt on 2/9/17.
//

#ifndef OS_LAB1_MBUF_ALLOC_H
#define OS_LAB1_MBUF_ALLOC_H

#include "mbuf.h"

MBuf* private_mbuf_alloc();
void private_mbuf_free(MBuf *mBuf);

#define MBUF_NUM (1<<13)

#endif //OS_LAB1_MBUF_ALLOC_H
