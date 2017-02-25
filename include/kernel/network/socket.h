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

typedef struct sockaddr {
    uint8_t sa_len;
    uint8_t sa_famliy;
    char sa_data[14]; // actually loner; address value
};

struct osockaddr {
    uint8_t sa_family;
    char sa_data[14];
};

typedef enum {

} ESocketType;

typedef struct {
    Inpcb *so_pcb;
    ESocketType so_type;
} Socket;

#endif //OS_LAB1_SOCKET_H
