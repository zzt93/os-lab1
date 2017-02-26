//
// Created by zzt on 2/26/17.
//

#ifndef OS_LAB1_INTERFACE_ETHER_H
#define OS_LAB1_INTERFACE_ETHER_H

typedef struct arpcom {
    NetworkInterface ac_if; // network-visible interface
    uint8_t ac_enaddr[6]; // ethernet hardware address
    InterfaceAddr ac_ipaddr;
    struct ether_multi *ac_multiaddrs;
    int ac_multicnt; // len of ac_multiaddrs list
};

#endif //OS_LAB1_INTERFACE_ETHER_H
