#include "kernel/syscall.h"
#include "kernel/manager/manager.h"
#include "kernel/message.h"
#include "kernel/process.h"

#include "drivers/hal.h"

/*
int __attribute__((__noinline__))
syscall(int id, ...) {
	int ret;
	int *args = &id;
	asm volatile("int $0x80": "=a"(ret) : "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]));
	return ret;
}
*/

void do_syscall(TrapFrame *tf) {
	int id = tf->eax; // system call id
    Msg m;
    m.src = current->pid;
    NOINTR;

    if (id < FM_PM) {
        switch (id) {
            case SYS_read:
                //send(FM, m);
                //receive(FM, m);
                //int nread = m.ret;
                //tf->eax = nread;   // return value is stored in eax
                break;
            case SYS_write:
                //tf->eax = nwrite;
                break;
            default:
                printk(RED"no such system call %d "RESET, id);
                assert(0);
        }
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
            case SYS_wait:
                m.type = NEW_TIMER;
                m.i[0] = tf->ebx;
                m.i[1] = current->pid;
                send(TIMER, &m);
                receive(TIMER, &m);
                break;
            case SLEEP:
                break;
            default:
                printk(RED"no such system call %d "RESET, id);
                assert(0);
        }
    }
}