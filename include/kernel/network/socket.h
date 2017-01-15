//
// Created by zzt on 1/15/17.
//

#ifndef OS_LAB1_SOCKET_H
#define OS_LAB1_SOCKET_H

#include "inpcb.h"

typedef enum {

} ESocketType;

typedef struct {
    Inpcb *so_pcb;
    ESocketType so_type;
} Socket;

#endif //OS_LAB1_SOCKET_H
