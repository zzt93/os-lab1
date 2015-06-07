#include "kernel/syscall.h"
#include "kernel/manager.h"
#include "kernel/message.h"


int __attribute__((__noinline__))
syscall(int id, ...) {
	int ret;
	int *args = &id;
	asm volatile("int $0x80": "=a"(ret) : "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]));
	return ret;
}

void do_syscall(TrapFrame *tf) {
	int id = tf->eax; // system call id

	switch (id) {
		case SYS_read:
			send(FM, m);
			receive(FM, m);
			int nread = m.ret;
			tf->eax = nread;   // return value is stored in eax
			break;
		case SYS_write:
			tf->eax = nwrite;
			break;
        default:
            assert(0);
	}
}