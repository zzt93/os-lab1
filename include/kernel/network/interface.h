//
// Created by zzt on 1/16/17.
//

#ifndef OS_LAB1_INTERFACE_H
#define OS_LAB1_INTERFACE_H

#include <drivers/time.h>
#include "types.h"
#include "socket.h"
#include "lib/malloc.h"


struct mbuf;
typedef struct mbuf MBuf;
struct ifaddr;
typedef struct ifaddr InterfaceAddr;

typedef enum {
    /*
     * kernel only, except `IFF_LOOPBACK`
     */

            IFF_BROADCAST,
    IFF_MULTICAST,
    IFF_POINT_TO_POINT,
    IFF_LOOPBACK,
    IFF_OACTIVE, // transmission is in progress
    IFF_RUNNING, // resources are allocated for this interface
    IFF_SIMPLEX, // the interface cannot receive its transmissions
    /*
     * according to text
     */

            IFF_LINK0,
    IFF_LINK1,
    IFF_LINK2,
    /*
     * ?
     */

            IFF_ALLMULTI, // the interface is receiving all multicast packet
    IFF_DEBUG, // debugging is enabled for the interface
    IFF_NO_ARP, // don't use arp on this interface
    IFF_NO_TRAILERS, // avoid using trailer encapsulation
    IFF_PROMICS, // the interface receivers all network packets
    IFF_UP, // the interface is operating
} EIterfaceFlags;

#define IFF_CANT_CHANGE \
(IFF_BROADCAST|IFF_MULTICAST|IFF_POINT_TO_POINT|IFF_OACTIVE|IFF_RUNNING|IFF_SIMPLEX)

typedef struct ifqueue {
    MBuf *ifq_head;
    MBuf *ifq_tail;
    int ifq_len; // current len of queue
    int ifq_maxlen; // maximum len of queue
    int ifq_drops; // packets dropped because of full queue
} InterfaceQueue;

#include "if_types.h"

typedef struct ifnet {
    /*
     * ------ implementation information -----------
     */
    struct ifnet *if_next;
    InterfaceAddr *if_addrlist;

    /*
     * In addition to a numeric index,
     * each interface has a text name formed from the if_name and if_unit members of the ifnet structure
     */
    char *if_name;
    short if_unit; // sub-unit for lower level driver
    uint16_t if_index; // uniquely identifies the interface

    short if_flags;
    short if_timer; // time until if_watchdog called in seconds
    int if_pcount; // number of promiscuous listeners
    caddr_t if_bpf; // packet filter structure

    struct if_data {
        /*
         * generic interface information
         */
        uint8_t if_type;
        uint8_t if_addrlen; // media address length
        uint8_t if_hdrlen; // media header length
        u_long if_mtu;
        u_long if_metric; // routing metric (external only)
        u_long if_baudrate; // line speed
        /*
         * volatile statistics
         */
        u_long if_ipackets;
        u_long if_ierrors;
        u_long if_opackets;
        u_long if_oerrors;
        u_long if_collisions; // collision on CSMA interfaces
        u_long if_ibytes;
        u_long if_obytes;
        u_long if_imcasts;
        u_long if_omcasts;
        u_long if_idrops; // packets drops on input
        u_long if_no_protocol;// packets destined for unsupported protocol
        Time if_lastchange;
    } if_data;

    /*
     * procedure handles
     */
    int (*if_init)(int); // init routine
    int (*if_output)
            (struct ifnet *, MBuf *, SockAddr *, struct rtentry *);

    int (*if_start)(struct ifnet *); // initiate output routine
    int (*if_done)(struct ifnet *); // output complete routine
    int (*if_ioctl)(struct ifnet *, int, caddr_t); // io control routine

    int (*if_reset)(int); // new autoconfig will permit removal
    int (*if_watchdog)(int); // timer routine

    InterfaceQueue if_send;
} NetworkInterface;

#define IFQ_MAXLEN 50

typedef struct ifqueue IFQueue;

int if_qfull(IFQueue *ifq);

void if_inc_dropcount(IFQueue *ifq);

void if_enqueue(IFQueue *ifQueue, MBuf *m);

void if_prepend(IFQueue *ifQueue, MBuf *mBuf);

void if_dequeue(IFQueue *ifQueue, MBuf *m);

void if_qflush(IFQueue *ifQueue);

typedef struct ifaddr {
    struct ifaddr *ifa_next; // next address for this interface
    NetworkInterface *ifa_to_if; // back pointer to interface
    SockAddr *ifa_addr; // address
    SockAddr *ifa_p2p_dest_addr; // other end of p-to-p link
#define ifa_broadaddr ifa_p2p_dest_addr /*broadcast address on a broadcast network such as ethernet*/
    SockAddr *ifa_netmask;

    void (*ifa_router_request)(); // check or clean routes
    uint16_t ifa_flags;
    short ifa_ref_cnt; // reference count to this address: shared by interface and routing
    int ifa_metric; // cost for this interface
} InterfaceAddr;


/**
 * complete initialization of the interface's ifnet structure;
 * and to insert the structure on the list of previously configured interfaces;
 * the kernel initializes and assigns each interface a link-level address
 */
void if_attach(NetworkInterface *);

void bpf_attach(caddr_t bpf, NetworkInterface *networkInterface, int, int);

#endif //OS_LAB1_INTERFACE_H
