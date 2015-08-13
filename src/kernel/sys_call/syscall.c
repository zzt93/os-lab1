#include "kernel/syscall.h"
#include "kernel/manager/manager.h"
#include "kernel/message.h"
#include "kernel/process.h"

#include "drivers/hal.h"
#include "drivers/tty/tty4.h"


/*
int __attribute__((__noinline__))
syscall(int id, ...) {
	int ret;
	int *args = &id;
	asm volatile("int $0x80": "=a"(ret) : "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]));
	return ret;
}
*/

/**
   NOTICE:
   if the parameters contains address, may be you need physical address -- but not  convert it now, convert it after receiving message
 */
void do_syscall(TrapFrame *tf) {
	int id = tf->eax; // system call id
    Msg m;
    m.src = current->pid;
    NOINTR;

    if (id < FM_PM) {
        switch (id) {
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
                    INVALID_ID, tf->ebx, tf->ecx, 0, tf->edx);
                break;
            case SYS_write:
                // tf->ebx -- fd
                // tf->ecx -- buffer
                // tf->edx -- len
                init_msg(&m,
                    current->pid,
                    FM_write,
                    INVALID_ID, tf->ebx, tf->ecx, 0, tf->edx);
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
                m.buf = current;
                // old_fd
                m.i[0] = tf->ebx;
                // new_fd in dev_id
                m.i[1] = tf->ecx;
                break;
            default:
                printk(RED"no such system call %d "RESET, id);
                assert(0);
        }
        send(FM, m);
        receive(FM, m);
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
                tf->eax = dev_read(TTY4,
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