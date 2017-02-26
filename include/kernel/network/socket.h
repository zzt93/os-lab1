//
// Created by zzt on 1/15/17.
//

#ifndef OS_LAB1_SOCKET_H
#define OS_LAB1_SOCKET_H

#include "inpcb.h"

typedef enum {
    AF_INET, // internet
    AF_ISO, AF_OSI, // OSI
    AF_UNIX, // Unix
    AF_ROUTE, // routing table
    AF_LINK, // data link
    AF_UNSPEC, // depend on text; In most case, it contains an Ethernet hardware address
} AddressFamily;

typedef struct sockaddress {
    uint8_t sa_len;
    uint8_t sa_famliy;
    /*
     * This is a common C technique that allows the programmer
     * to consider the last member in a structure to have a variable length.
     */
    char sa_data[14]; // actually loner; address value
} SockAddr;

typedef enum {

} ESocketType;

typedef struct {
    Inpcb *so_pcb;
    ESocketType so_type;
} Socket;

#endif //OS_LAB1_SOCKET_H
