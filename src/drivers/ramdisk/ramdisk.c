#include "drivers/hal.h"
#include "drivers/ramdisk/ramdisk.h"
#include "kernel/process.h"
#include "kernel/init_proc.h"
#include "kernel/message.h"

pid_t RAM_DISK;

static void ram_disk_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        switch(m.type) {
            case DEV_READ:
                read_ram(&m);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void init_ramdisk() {
    PCB* p = create_kthread(ram_disk_job);
    RAM_DISK = p->pid;
    hal_register(ram, RAM_DISK, 0);
    add2wake(p);
    init_ram();
}