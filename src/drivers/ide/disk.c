#include "common.h"
#include "x86/x86.h"
#include "kernel/message.h"
#include "drivers/hal.h"
#include "ide.h"

#define IDE_PORT_CTRL   0x3F6
#define IDE_PORT_BASE   0x1F0

static void
ide_prepare(uint32_t sector) {
    while ((in_byte(IDE_PORT_BASE + 7) & (0x80 | 0x40)) != 0x40) {
        printk("%s, %d:IDE NOT READY\n", __FUNCTION__, __LINE__);
    }

    out_byte(IDE_PORT_CTRL, 0);
    out_byte(IDE_PORT_BASE + 2, 1);
    out_byte(IDE_PORT_BASE + 3, sector & 0xFF);
    out_byte(IDE_PORT_BASE + 4, (sector >> 8) & 0xFF);
    out_byte(IDE_PORT_BASE + 5, (sector >> 16) & 0xFF);
    out_byte(IDE_PORT_BASE + 6, 0xE0 | ((sector >> 24) & 0xFF));
}

static inline void
issue_read() {
    out_byte(IDE_PORT_BASE + 7, 0x20);
}

static inline void
issue_write() {
    out_byte(IDE_PORT_BASE + 7, 0x30);
}

/**
seems like the DMA way, ide_prepare() send the message about reading,
when it finished, send a interrupt to CPU and to wake up ide to handle
it.
*/
void
disk_do_read(void *buf, uint32_t sector) {
    int i;
    Msg m;

    ide_prepare(sector);
    issue_read();

    do {
        receive(MSG_HARD_INTR, &m);
    } while (m.type != IDE_READY);

    for (i = 0; i < 512 / sizeof(uint32_t); i++) {
        *(((uint32_t *) buf) + i) = in_long(IDE_PORT_BASE);
    }
}

void
disk_do_write(void *buf, uint32_t sector) {
    int i;
    Msg m;

    ide_prepare(sector);
    issue_write();

    for (i = 0; i < 512 / sizeof(uint32_t); i++) {
        out_long(IDE_PORT_BASE, *(((uint32_t *) buf) + i));
    }
    do {
        receive(MSG_HARD_INTR, &m);
    } while (m.type != IDE_READY);
}

