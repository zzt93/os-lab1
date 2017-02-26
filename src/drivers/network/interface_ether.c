//
// Created by zzt on 2/26/17.
//

#define NLE 1

struct	le_softc {
    struct	arpcom sc_ac;	/* common Ethernet structures */
#define	sc_if	sc_ac.ac_if	/* network-visible interface */
#define	sc_addr	sc_ac.ac_enaddr	/* hardware Ethernet address */
    volatile struct	lereg1 *sc_r1;	/* LANCE registers */
    volatile void *sc_r2;	/* dual-port RAM */
    int	sc_ler2pad;	/* Do ring descriptors require short pads? */
    void	(*sc_copytobuf)(); /* Copy to buffer */
    void	(*sc_copyfrombuf)(); /* Copy from buffer */
    void	(*sc_zerobuf)(); /* and Zero bytes in buffer */
    int	sc_rmd;		/* predicted next rmd to process */
    int	sc_tmd;		/* last tmd processed */
    int	sc_tmdnext;	/* next tmd to transmit with */
    /* stats */
    int	sc_runt;
    int	sc_merr;
    int	sc_babl;
    int	sc_cerr;
    int	sc_miss;
    int	sc_rown;
    int	sc_xint;
    int	sc_uflo;
    int	sc_rxlen;
    int	sc_rxoff;
    int	sc_txoff;
    int	sc_busy;
    short	sc_iflags;
} le_softc[NLE];