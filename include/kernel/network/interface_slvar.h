//
// Created by zzt on 2/28/17.
//

#ifndef OS_LAB1_INTERFACE_SLVAR_H
#define OS_LAB1_INTERFACE_SLVAR_H

#include <drivers/tty/tty.h>
#include "interface.h"

typedef enum {
    SC_COMPRESS, //sc_if.if_flags = IFF_LINK0; compress TCP traffic
    SC_NOICMP, // sc_if.if_flags = IFF_LINK1; suppress ICMP traffic
    SC_AUTOCOMP, // sc_if.if_flags = IFF_LINK2; auto-enable TCP compression
    SC_ERROR, // error detected; discard incoming frame
};

typedef struct sl_softc {
    NetworkInterface sc_if;
    // interactive output queue, which require low latency
    InterfaceQueue sc_interactive_out_q;
    Console *sc_tty;
    uint8_t *sc_mp; // pointer to next available buf char of SLIP packet
    uint8_t *sc_ep; // pointer to last available buf char of SLIP packet
    uint8_t *sc_buf; // input buf
    uint32_t sc_flags;
    uint32_t sc_escape; // =1 if last char input was FRAME_ESCAPE
    struct sl_compress sc_compress; // tcp compression data
    caddr_t sc_bpf; // BPF data
} SlSoft;

void sl_attach(int n);

#define NSL 1
// 20-byte IP header, 20-byte TCP, 256 bytes user data
#define SL_MTU 296
int (*sl_ioctl)(struct ifnet *, int, caddr_t); // io control routine
int (*sl_output)
        (struct ifnet *, MBuf *, SockAddr *, struct rtentry *);

#endif //OS_LAB1_INTERFACE_SLVAR_H
