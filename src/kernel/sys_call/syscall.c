#include "kernel/syscall.h"
#include "kernel/manager/manager.h"
#include "kernel/message.h"
#include "kernel/process.h"

#include "drivers/hal.h"
#include "drivers/tty/tty4.h"

#include "kernel/init_proc.h"



int to_ddl_get(int pid);
int to_ddl_update(int pid, int to_ddl);
/**
   NOTICE:
   if the parameters contains address, may be you need
   physical address to make cross-process exchange message
   -- but not convert it now,
   convert it after receiving message
   ie, in the target process/server
 */
void do_syscall(TrapFrame *tf) {
	int id = tf->eax; // system call id
    Msg m;
    m.src = current->pid;
    NOINTR;

    if (id < FM_PM) {
        switch (id) {
            case SYS_create:
                m.type = FM_create;
                // name of file
                m.dev_id = tf->ebx;
                m.buf = current;
                break;
            case SYS_make:
                m.type = FM_make;
                // name of directory
                m.dev_id = tf->ebx;
                m.buf = current;
                break;
            case SYS_del:
                m.type = FM_del;
                // name of file
                m.dev_id = tf->ebx;
                m.buf = current;
                break;
            case SYS_open:
                m.type = FM_open;
                // file name
                m.dev_id = tf->ebx;
                m.buf = current;
                break;
            case SYS_read:
                // TODO tf->ebx now is fd, but
                // read_ram() treat it as file name
                // tf->ebx -- fd
                // tf->ecx -- buffer
                // tf->edx -- len
                init_msg(&m,
                    current->pid,
                    FM_read,
                    (pid_t)current, tf->ebx, (void *)tf->ecx, 0, tf->edx);
                break;
            case SYS_write:
                // tf->ebx -- fd
                // tf->ecx -- buffer
                // tf->edx -- len
                init_msg(&m,
                    current->pid,
                    FM_write,
                    (pid_t)current, tf->ebx, (void *)tf->ecx, 0, tf->edx);
                break;
            case SYS_close:
                m.type = FM_close;
                m.buf = current;
                // dev_id is fd
                m.dev_id = tf->ebx;
                break;
            case SYS_lseek:
                m.type = FM_lseek;
                // dev_id is fd
                m.dev_id = tf->ebx;
                m.offset = tf->ecx;
                m.buf = current;
                // len represent whence
                m.len = tf->edx;
                break;
            case SYS_dup:
                m.type = FM_dup;
                m.buf = current;
                // here dev_id represent file descriptor
                m.dev_id = tf->ebx;
                break;
            case SYS_dup2:
                m.type = FM_dup2;
                m.i[2] = (int)current;
                // old_fd
                m.i[0] = tf->ebx;
                // new_fd in dev_id
                m.i[1] = tf->ecx;
                break;
            case SYS_lsdir:
                m.type = FM_lsdir;
                // name
                m.dev_id = tf->ebx;
                m.buf = current;
                // the target buffer
                m.req_pid = tf->ecx;
            case SYS_chdir:
                m.type = FM_chdir;
                // name
                m.dev_id = tf->ebx;
                m.buf = current;
            default:
                printk(RED"no such system call %d "RESET, id);
                assert(0);
        }
        send(FM, &m);
        receive(FM, &m);
        tf->eax = m.ret;
    } else if (id < MIS) {
        switch (id) {
            case SYS_fork:
                m.type = PM_fork;
                // current PCB*
                m.buf = current;
                break;
            case SYS_exec:
                m.type = PM_exec;
                // file name -- int
                m.i[0] = tf->ebx;
                // current PCB*
                m.i[1] = (int)current;
                // args address
                m.buf = (void *)tf->ecx;
                break;
            case SYS_exit:
                m.type = PM_exit;
                // current PCB*
                m.buf = current;
                break;
            case SYS_getpid:
                tf->eax = current->pid;
                return;
            case SYS_waitpid:
                m.type = PM_waitpid;
                // which process current process wait for
                m.i[0] = tf->ebx;
                m.buf = current;
                break;
            case SYS_set_priority:
                if (priority_in_range(tf->ebx)) {
                    kset_edf_priority(current, tf->ebx);
                    tf->eax = SUCC;
                } else {
                    tf->eax = INVALID_PRI;
                }
                return;
            case SYS_get_priority:
                tf->eax = kget_priority(current);
                return;
            case SYS_add_task_ddl:
                // notice:
                // this method is also used to put initial ddl
                // to map
            {
                int new_ddl = to_ddl_get(current->pid) + tf->ebx;
                tf->eax = to_ddl_update(current->pid, new_ddl);
				current->priority = USER_PRI - new_ddl;
				// no need to find it in the heap for it not in the heap when running
//                kset_edf_priority(current, USER_PRI - new_ddl);
                return;
            }
            case SYS_using_edf:
                tf->eax = kusing_edf();
                return;
            default:
                printk(RED"no such system call %d "RESET, id);
                assert(0);
        }
        send(PM, &m);
        receive(PM, &m);
        tf->eax = m.ret;
    } else {
        switch (id) {
            case 111:
                printk(RED"user process test "RESET);
                break;
            case SYS_print_serial:
                printk(RED"%s "RESET, tf->ebx);
                break;
            case SYS_printf:
                kprintf((const char *)tf->ebx, (void **)tf->ecx);
                break;
            case SYS_read_line:
                tf->eax = n_dev_read(d_ttyi[NOW_TERMINAL],
                    current->pid,
                    (void *)tf->ebx,
                    0, tf->ecx);
                break;
            case SYS_wait:
                m.type = NEW_TIMER;
                m.i[0] = tf->ebx;
                m.i[1] = current->pid;
                send(TIMER, &m);
                receive(TIMER, &m);
                break;
            case SYS_set_timer:
                m.type = NEW_TIMER;
                m.i[0] = tf->ebx;
                m.i[1] = current->pid;
                send(TIMER, &m);
                break;
            case SYS_wait_timer:
                receive(TIMER, &m);
                break;
            case SYS_timer_start:
                tf->eax = kstart_non_block_timer(tf->ebx);
                break;
            case SYS_timer_finished:
                tf->eax = knon_block_timer_finished();
                break;
            case SYS_prompt:
                m.type = PROMPT;
                send(TTY, &m);
                receive(TTY, &m);
                tf->eax = m.ret;
                break;
            case SLEEP:
                break;
            default:
                printk(RED"no such system call %d "RESET, id);
                assert(0);
        }
    }
}
