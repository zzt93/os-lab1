#include "drivers/hal.h"
#include "drivers/ramdisk/ramdisk.h"
#include "kernel/process.h"
#include "kernel/init_proc.h"
#include "kernel/message.h"

pid_t RAM_DISK;
int d_ramdisk;

static void ram_disk_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        pid_t dest = m.src;
        switch(m.type) {
            case DEV_READ:
                m.ret = read_ram(&m);
                break;
            default:
                assert(false);
                break;
        }
        m.src = RAM_DISK;
        send(dest, &m);
    }
}

void init_ramdisk() {
    PCB* p = create_kthread(ram_disk_job);
    RAM_DISK = p->pid;
    hal_register(ram, RAM_DISK, &d_ramdisk);
    add2wake(p);
    init_ram();
}