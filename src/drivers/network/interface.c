//
// Created by zzt on 2/21/17.
//


#include <kernel/network/interface.h>
#include <kernel/network/interface_dl.h>
#include <lib/string.h>
#include <kernel/semaphore.h>

InterfaceAddr *ifnet_addrs[];
static uint16_t if_index = 0;
NetworkInterface *if_head;

void ether_if_attach(NetworkInterface *ifp);

void if_slow_timeout(void *arg);

void if_attach(NetworkInterface *ifp) {
    unsigned socksize, ifaddr_size;
    uint8_t namelen, unitlen, masklen;
    int ether_output();
    char workbuf[12], *unitname;
    NetworkInterface **p = &if_head;
    SockAddrDataLink *sockAddrDataLink;
    InterfaceAddr *ifaddr;
    static int if_indexlim = 8; // size of InterfaceAddr array
    extern void link_rt_request();

    // find the end of interface list
    while (*p) {
        p = &((*p)->if_next);
    }
    *p = ifp;
    ifp->if_index = ++if_index;

    // resize InterfaceAddr array if necessary
    if (ifnet_addrs == 0 || if_index >= if_indexlim) {
        unsigned n = (if_indexlim <<= 1) * sizeof(ifaddr);
        InterfaceAddr **q = (InterfaceAddr **) malloc(n, M_IFADDR, M_WAITOK);

        if (ifnet_addrs) {
            memcpy((caddr_t) q, (caddr_t) ifnet_addrs, n / 2);
            kfree(ifnet_addrs);
        }
        ifnet_addrs = q;
    }

    // create link-level name for this device
    itoa_s((uint32_t) ifp->if_unit, workbuf, sizeof(workbuf));
    unitname = workbuf;
    size_t len = strlen(ifp->if_name);
    assert(len <= 0xff);
    namelen = (uint8_t) len;
    unitlen = (uint8_t) strlen(unitname);

    // compute size of sockaddr_dl structure for this device
#define _offsetof(t, m) ((int)((caddr_t)&((t *)0)->m))
    masklen = (uint8_t) (_offsetof(SockAddrDataLink, sdl_data[0]) + unitlen + namelen);
    socksize = masklen + ifp->if_data.if_addrlen;
#define ROUND_UP(a) (1 + (((a) - 1) | (sizeof(long) - 1)))
    socksize = ROUND_UP(socksize);
    if (socksize < sizeof(*sockAddrDataLink)) {
        socksize = sizeof(*sockAddrDataLink);
    }
    ifaddr_size = sizeof(*ifaddr) + 2 * socksize;

    // allocate and init link-level address
    if ((ifaddr = malloc(ifaddr_size, M_IFADDR, M_WAITOK)) != NULL) {
        memset(ifaddr, 0, ifaddr_size);

        // init the SockAddrDataLink address
        sockAddrDataLink = (SockAddrDataLink *) (ifaddr + 1);
        assert(socksize <= 0xff);
        sockAddrDataLink->sdl_len = (uint8_t) socksize;
        sockAddrDataLink->sdl_family = AF_LINK;
        // data = if_name + unit_name
        memcpy(sockAddrDataLink->sdl_data, ifp->if_name, namelen);
        memcpy((caddr_t)sockAddrDataLink->sdl_data + namelen, unitname, unitlen);
        sockAddrDataLink->sdl_name_len = (uint8_t) (namelen+=unitlen);

        sockAddrDataLink->sdl_index = ifp->if_index;
        sockAddrDataLink->sdl_type = ifp->if_data.if_type;
        ifnet_addrs[if_index - 1] = ifaddr;
        ifaddr->ifa_to_if = ifp;
        // insert this address
        ifaddr->ifa_next = ifp->if_addrlist;
        ifp->if_addrlist = ifaddr;

        ifaddr->ifa_router_request = link_rt_request;
        ifaddr->ifa_addr = (SockAddr *) sockAddrDataLink;

        // init the SockAddrDataLink mask
        sockAddrDataLink = (SockAddrDataLink *) ((caddr_t )sockAddrDataLink + socksize);
        ifaddr->ifa_netmask = (SockAddr *) sockAddrDataLink;
        sockAddrDataLink->sdl_len = masklen;
        while (namelen != 0) {
            sockAddrDataLink->sdl_data[--namelen] = (char) 0xff;
        }

        /* XXX -- Temporary fix before changing 10 ethernet drivers */
        if (ifp->if_output == ether_output) {
            ether_if_attach(ifp);
        }
    }
}

void if_init() {
    NetworkInterface *ifp;

    for(ifp = ifnet_addrs; ifp; ifp = ifp->if_next) {
        if (ifp->if_send.ifq_maxlen == 0) {
            ifp->if_send.ifq_maxlen = IFQ_MAXLEN;
        }
    }
    if_slow_timeout(0);
}

void if_slow_timeout(void *arg) {
    NetworkInterface *ifp;
    lock();

    for (ifp = ifnet_addrs; ifp; ifp = ifp->if_next) {
        if (ifp->if_timer == 0 || --ifp->if_timer) {
            continue;
        }
        if (ifp->if_watchdog) {
            (*ifp->if_watchdog)(ifp->if_unit);
        }
    }
    unlock();
//    timeout(if_slow_timeout, NULL, HZ/IFNET_SLOWHZ);
}