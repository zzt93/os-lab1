//
// Created by zzt on 2/9/17.
//

#include <kernel/network/mbuf.h>
#include <kernel/network/cluster.h>


MCluster mcluster_start[M_CLUSTER_NUM];
char *mclrefcnt;


void mcluster_get(MBuf *m, EMemoryAllocWait nowait) {

}


/*
 * -------------- static method ------------------
 */
static
MCluster *mcluster_alloc(EMemoryAllocWait how) {
    MBUF_LOCK(
            if () {

            }
    );
}