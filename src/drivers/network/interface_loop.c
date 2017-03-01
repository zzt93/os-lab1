//
// Created by zzt on 3/1/17.
//
#include <kernel/network/interface_loop.h>


void loop_attach(int n) {
    NetworkInterface *ifp = &loif;

    ifp->if_name = "lo";
    ifp->if_data.if_mtu = LO_MTU;
    ifp->if_flags = IFF_LOOPBACK | IFF_MULTICAST;
    ifp->if_ioctl = lo_ioctl;
    ifp->if_output = lo_output;
    ifp->if_data.if_type = IFT_LOOP;
    ifp->if_data.if_hdrlen = 0;
    ifp->if_data.if_addrlen = 0;
    if_attach(ifp);
//    bpf_attach(&ifp->if_bpf, ifp, , sizeof(uint32_t));
}

