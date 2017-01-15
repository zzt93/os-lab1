//
// Created by zzt on 1/15/17.
//

#include <types.h>

#ifndef OS_LAB1_NETWORK_H
#define OS_LAB1_NETWORK_H

typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

/* Address to accept any incoming messages.  */
#define	INADDR_ANY		((in_addr_t) 0x00000000)
/* Address to send to all hosts.  */
#define	INADDR_BROADCAST	((in_addr_t) 0xffffffff)

#endif //OS_LAB1_NETWORK_H
