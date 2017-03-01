//
// Created by zzt on 2/26/17.
//

#ifndef OS_LAB1_INTERFACE_ETHER_H
#define OS_LAB1_INTERFACE_ETHER_H

#include "interface.h"

typedef struct arpcom {
    NetworkInterface ac_if; // network-visible interface
    /*
     * ac_enaddr is the Ethernet hardware address copied by the LANCE device driver from the
     * hardware when the kernel locates the device during cpu_startup.
     */
    uint8_t ac_enaddr[6]; // ethernet hardware address
    InterfaceAddr ac_ipaddr;
    struct ether_multi *ac_multiaddrs;
    int ac_multicnt; // len of ac_multiaddrs list
} ARPCommon;

#define ETHER_MTU 1500
int (*le_init)(int); // init routine
int (*ether_output)(struct ifnet *, MBuf *, SockAddr *, struct rtentry *);
int (*le_ioctl)(struct ifnet *, int, caddr_t); // io control routine
int (*le_reset)(int); // new autoconfig will permit removal
int (*le_start)(struct ifnet *); // initiate output routine


#endif //OS_LAB1_INTERFACE_ETHER_H
