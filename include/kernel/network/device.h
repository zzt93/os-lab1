//
// Created by zzt on 2/26/17.
//

#ifndef OS_LAB1_DEVICE_H
#define OS_LAB1_DEVICE_H

typedef struct pdevinit {
    void (*pdev_attach)(int); // device attach method
    int pdev_count;
} PseudoDeviceInit;

#endif //OS_LAB1_DEVICE_H
