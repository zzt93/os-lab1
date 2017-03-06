//
// Created by zzt on 2/21/17.
//


#include <kernel/network/interface_slvar.h>

SlSoft sl_softc[NSL];

void sl_attach(int n) {
    SlSoft *sc;
    int i = 0;
    for (sc = sl_softc; i < NSL; sc++, i++) {
        sc->sc_if.if_name = "sl";
        sc->sc_if.if_next = NULL;
        sc->sc_if.if_unit = i;
        sc->sc_if.if_data.if_mtu = SL_MTU;
        sc->sc_if.if_flags =
                IFF_POINT_TO_POINT | SC_AUTOCOMP | IFF_MULTICAST;
        sc->sc_if.if_data.if_type = IFT_SLIP;
        sc->sc_if.if_ioctl = sl_ioctl;
        sc->sc_if.if_output = sl_output;
        sc->sc_if.if_send.ifq_maxlen = 50;
        sc->sc_fastq.ifq_maxlen = 32;
        if_attach(&sc->sc_if);
//        bpf_attach()
    }
}