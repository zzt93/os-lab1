//
// Created by zzt on 3/1/17.
//

#ifndef OS_LAB1_INTERFACE_DL_H
#define OS_LAB1_INTERFACE_DL_H

/*
 * Structure of a Link-Level sockaddr:
 */
typedef struct sockaddr_data_link {
    uint8_t sdl_len; // total length of sock addr
    uint8_t sdl_family; // AF_LINK
    /*
     * Ethernet interface would have an index of 1, the SLIP interface an index of 2,
     * and the loopback interface an index of 3. The global integer if_index contains the last index assigned by the kernel.
     */
    uint16_t sdl_index; // if != 0, system given index for interface
    uint8_t sdl_type; // interface type
    uint8_t sdl_name_len; // interface name len, no trialing 0 required
    uint8_t sdl_addr_len; // link level address len
    uint8_t sdl_selector_len; // link level selector len
    char sdl_data[12]; // minimum work area, can be larger; contain both interface name and ll address
} SockAddrDataLink;

#define LLADDR(s) ((caddr_t)((s)->sdl_data + (s)->sdl_name_len))

#endif //OS_LAB1_INTERFACE_DL_H
