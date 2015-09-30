#include "kernel/process.h"
#include "kernel/manager/FM.h"
#include "kernel/message.h"

#include "kernel/manager/fd.h"
#include "kernel/manager/f_dir.h"
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
        int type = m.type;
        m.ret = FM_ERR;
        switch(type) {
            case FM_read:
                read_file(&m);
                //n_read_file(&m);
                break;
            case FM_write:
                write_file(&m);
                break;
            case FM_open:
                open_file(&m);
                break;
            case FM_close:
                close_file(&m);
                break;
            case FM_dup:
                dup_file(&m);
                break;
            case FM_dup2:
                dup2_file(&m);
                break;
            case FM_lseek:
                lseek_file(&m);
                break;
            case FM_create:
                create_file(&m);
                break;
            case FM_make:
                make_dir(&m);
                break;
            case FM_del:
                delete_file(&m);
                break;
            case FM_lsdir:
                list_dir(&m);
                break;
            case FM_chdir:
                ch_dir(&m);
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
    now_disk = d_ide;
    // init each region start and size
    load_super_block();
    /**
       default system opened file -- load_super_block() have to
       before init_file_table(); for file_table will use inode_start
    */
    init_file_table();
    // for many thread is already initialized with `0`, so
    // re-init them
    init_thread_cwd();
}

void init_FM() {
    PCB* p = create_kthread(FM_job);
    FM = p->pid;
    add2wake(p);
}


