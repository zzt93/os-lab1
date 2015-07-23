#include "kernel/syscall.h"
#include "kernel/manager/manager.h"
#include "kernel/message.h"
#include "kernel/process.h"

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
                m.buf = current;
                break;
            case SYS_exec:
                m.type = PM_exec;
                m.i[0] = tf->ebx;
                m.i[1] = (int)current;
                m.buf = (void *)tf->ecx;
                break;
            case SYS_exit:
                m.type = PM_exit;
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
                printk("%s ", tf->ebx);
                break;
            default:
                printk(RED"no such system call %d "RESET, id);
                assert(0);
        }
    }
}