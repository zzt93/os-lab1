#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/message.h"

#include "kernel/manager/fd.h"
#include "kernel/manager/file_table.h"
#include "drivers/hal.h"

int FM;

int now_disk;
/**
   The message sent to FM should specify:
   m->type -- FM_read
        m->buf
        m->dev_id -- file name
        m->offset
        m->len
        The message send from FM specify:
        m->ret -- len successfully read
 */
static void FM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        pid_t dest = m.src;
        switch(m.type) {
            case FM_read:
                read_file(&m);
                break;
            case FM_write:
                assert(false);
                break;
            case FM_open:
                m.ret = open_file(&m);
                break;
            case FM_close:
                m.ret = close_file(&m);
                break;
            case FM_dup:
                m.ret = dup_file(&m);
                break;
            case FM_dup2:
                m.ret = dup2_file(&m);
                break;
            case FM_lseek:
                m.ret = lseek_file(&m);
                break;
            default:
                assert(false);
                break;
        }
        m.src = FM;
        send(dest, &m);
    }
}

void load_super_block();


/**
   init by super block(start and size of each region)
   init inode bit map; init block bit map
   initialize STD*** in file table
 */
void init_file_system() {
    now_disk = d_ramdisk;
    // default system opened file
    init_file_table();
    // init each region start and size
    load_super_block();
}

void init_FM() {
    PCB* p = create_kthread(FM_job);
    FM = p->pid;
    add2wake(p);
    //init_file_system();
}


