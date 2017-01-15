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
   m->type -- FM_ram_read
        m->buf
        m->dev_id -- file name in int
        m->offset
        m->len

        The message send from FM specify:
        m->ret -- len successfully read

   m->type -- FM_read
        m->req_pid
        m->dev_id
        m->buf
        m->len
 */
static void FM_job() {
    static Msg m;

    while (true) {
        receive(ANY, &m);
        pid_t dest = m.src;
        int type = m.type;
        // default to be error, can be specified with
        // a detailed error or become a succ
        m.ret = FM_ERR;
        int res;
        switch(type) {
            case FM_ram_read:
                ram_read_file(&m);
                break;
            case FM_read:
                /**
                   @param m->req_pid -- (PCB *) request process, i.e. the process has buffer and fd
                   @param m->dev_id -- (int ) file descriptor
                   @param m->buf -- (char *) buffer to store read content
                 */
                n_read_file(&m);
                break;
            case FM_write:
                write_file(&m);
                break;
            case FM_open:
                /**
                   @param m->req_pid -- (PCB *) process own the name buffer
                   @param m->dev_id -- (char *) the program/file name
                   @param m->buf -- (PCB *) process want the fd
                   @return -- the fd if succeed
                */
                res = open_file(&m);
                SET_IF_SUCC(m, res);
                break;
            case FM_close:
                /**
                   @param m->buf -- (PCB *) process own the fd
                   @param m->dev_id -- (int) file fd
                   @return -- the fd
                 */
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
            {
                /**
                   @param m->req_pid -- (char *) buffer to store ls result
                   @param m->dev_id -- (char *) file name to list
                   @param m->buf -- (PCB *) process owning above name buffer and result buffer
                   @param m->len -- (int ) buffer length
                */
                int ls_res = list_dir(&m);
                SET_IF_SUCC(m, ls_res);
                break;
            }
            case FM_chdir:
                /**
                   @param m->buf -- (PCB *) process which want to change dir and own the name buffer
                   @param m->dev_id -- (char *) directory name
                 */
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
    // for many already initialized thread is initialized with
    // uinitialized default_cwd -- `0`, so
    // re-init cwd for them, @see init_proc.c:create_kthread();
    init_thread_cwd();
}

void init_FM() {
    PCB* p = create_kthread(FM_job);
    FM = p->pid;
    add2wake(p);
}
