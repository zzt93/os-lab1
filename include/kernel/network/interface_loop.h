//
// Created by zzt on 3/1/17.
//

#ifndef OS_LAB1_INTERFACE_LOOP_H
#define OS_LAB1_INTERFACE_LOOP_H

#include <kernel/network/interface.h>

#define LO_MTU 1526

int (*lo_ioctl)(struct ifnet *, int, caddr_t);
int (*lo_output)
        (struct ifnet *, MBuf *, SockAddr *, struct rtentry *);

#endif //OS_LAB1_INTERFACE_LOOP_H
