//
// Created by zzt on 2/22/17.
//

#ifndef OS_LAB1_IF_TYPES_H
#define OS_LAB1_IF_TYPES_H

typedef enum {
    IFT_OTHER,
    IFT_ETHER,
    IFT_ISO88023, // 802.3 Ethernet (CSMA/CD)
    IFT_LOOP, // loopback interface
    IFT_SLIP,
};

#endif //OS_LAB1_IF_TYPES_H
