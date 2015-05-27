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
            case RAM_READ:
                
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
    add2wake(p);
    init_ram();
}