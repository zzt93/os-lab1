//
// Created by zzt on 2/9/17.
//

#ifndef OS_LAB1_CLUSTER_H
#define OS_LAB1_CLUSTER_H

#include "mbuf.h"

#ifdef LARGER_MBUF
#define MCLBYTES 2048
#define MCLSHIFT 11
#else
#define MCLBYTES 1024
#define MCLSHIFT 10
#endif

#define MCLOFFSET (MCLBYTES - 1)

typedef union mcluster {
    union mcluster *mcl_next;
    char mcl_buf[MCLBYTES];
} MCluster;

extern MCluster *mcluster_start;        /* virtual address of mclusters */

#define M_CLUSTER_NUM 512

//  convert pointer within cluster to cluster index #
// i.e. x may be not the start of cluster, so can't just minus it with start
#define    mtocl(x)    (((u_long)(x) - (u_long)mcluster_start) >> MCLSHIFT)

void mcluster_get(MBuf *m, EMemoryAllocWait nowait);


#endif //OS_LAB1_CLUSTER_H
