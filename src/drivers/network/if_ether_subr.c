//
// Created by zzt on 3/8/17.
//

#include <kernel/network/interface.h>
#include <kernel/network/interface_dl.h>
#include <kernel/network/interface_ether.h>
#include <lib/string.h>

void ether_if_attach(NetworkInterface *ifp) {
    InterfaceAddr *ifa;
    SockAddrDataLink *sockAddrDataLink;

    ifp->if_data.if_type = IFT_ETHER;
    ifp->if_data.if_addrlen = 6;
    ifp->if_data.if_hdrlen = 14;
    ifp->if_data.if_mtu = ETHER_MTU;
    for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next) {
        if ((sockAddrDataLink = (SockAddrDataLink *) ifa->ifa_addr)
            && sockAddrDataLink->sdl_family == AF_LINK) {
            sockAddrDataLink->sdl_type = IFT_ETHER;
            sockAddrDataLink->sdl_addr_len = ifp->if_data.if_addrlen;
            memcpy(LLADDR(sockAddrDataLink), ((ARPCommon *) ifp)->ac_enaddr, ifp->if_data.if_addrlen);
            break;
        }
    }
}
