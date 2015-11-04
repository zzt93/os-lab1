#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/fd.h"


int first_fd(PCB *aim, void *value) {
    int i;
    for (i = 0; i < PROCESS_MAX_FD; i++) {
        if (aim->fd_table[i].ft_entry == value) {
            return i;
        }
    }
    return INVALID_FD_I;
}
