//
// Created by zzt on 2/26/17.
//

#include <kernel/network/interface.h>
#include <kernel/network/interface_lereg.h>
#include <kernel/network/interface_ether.h>

#define NLE 1

struct le_softc {
    struct arpcom sc_ac;    /* common Ethernet structures */
#define    sc_if    sc_ac.ac_if    /* network-visible interface */
#define    sc_addr    sc_ac.ac_enaddr    /* hardware Ethernet address */
    volatile struct lereg1 *sc_r1;    /* LANCE registers */
    volatile void *sc_r2;    /* dual-port RAM */
    int sc_ler2pad;    /* Do ring descriptors require short pads? */
    void (*sc_copytobuf)(); /* Copy to buffer */
    void (*sc_copyfrombuf)(); /* Copy from buffer */
    void (*sc_zerobuf)(); /* and Zero bytes in buffer */
    int sc_rmd;        /* predicted next rmd to process */
    int sc_tmd;        /* last tmd processed */
    int sc_tmdnext;    /* next tmd to transmit with */
    /* stats */
    int sc_runt;
    int sc_merr;
    int sc_babl;
    int sc_cerr;
    int sc_miss;
    int sc_rown;
    int sc_xint;
    int sc_uflo;
    int sc_rxlen;
    int sc_rxoff;
    int sc_txoff;
    int sc_busy;
    short sc_iflags;
} le_softc[NLE];


int le_attach(struct device *device) {
    LEReg0 *ler0;
    LEReg2 *ler2;
    LEReg2 *lemem = 0;
    struct le_softc *le = &le_softc[device->unit];
    NetworkInterface *ifp = &le->sc_ac.ac_if;
    char *cp;
    /*
     * device specific code
     */
    /*
     * read the ethernet address off the board, one nibble at a time
     */
    cp = (char *) (lestd[3] + (int) device->addr);
    for (int i = 0; i < sizeof(le->sc_ac.ac_enaddr); ++i) {
        le->sc_ac.ac_enaddr[i] = (*++cp & 0xf) << 4;
        cp++;
        le->sc_ac.ac_enaddr[i] |= *++cp & 0xf;
        cp++;
    }
    /*
     * device specific code
     */
    ifp->if_unit = device->unit;
    ifp->if_name = "le";
    ifp->if_data.if_mtu = ETHER_MTU;
    ifp->if_init = le_init;
    ifp->if_reset = le_reset;
    ifp->if_ioctl = le_ioctl;
    ifp->if_output = ether_output;
    ifp->if_start = le_start;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
//    bpf_attach(&ifp->if_bpf, ifp, );
    if_attach(ifp);
    return 1;
}