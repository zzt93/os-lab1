//
// Created by zzt on 1/15/17.
//

#ifndef OS_LAB1_INPCB_H
#define OS_LAB1_INPCB_H


typedef struct internet_protocol_control_block Inpcb;

struct internet_protocol_control_block {
    Inpcb *inp_next;
    Inpcb *inp_prev;
    in_addr_t inp_faddr;
    in_port_t inp_fport;
    in_addr_t inp_laddr;
    in_port_t inp_lport;
};
#endif //OS_LAB1_INPCB_H
