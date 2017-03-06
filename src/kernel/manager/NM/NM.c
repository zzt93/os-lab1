//
// Created by zzt on 1/15/17.
//

#include <kernel/network/device.h>
#include <const.h>
#include <macro.h>
#include "kernel/network/interface_slvar.h"
#include "kernel/network/interface_loop.h"

PseudoDeviceInit pseudoDeviceInit[] = {
        {sl_attach, 1},
        {loop_attach, 1},
};

void if_init();
void domain_init();


void attach_pseudo_dev();

void init_protocol();

void init_network() {
    attach_pseudo_dev();
    init_protocol();
}

void init_protocol() {
    NOINTR;
    // interface init
    if_init();
    // protocol domain init
    domain_init();
}

static
void attach_pseudo_dev() {
    for (PseudoDeviceInit* init = pseudoDeviceInit; init->pdev_attach != NULL; init++) {
        (*init->pdev_attach)(init->pdev_count);
    }
}