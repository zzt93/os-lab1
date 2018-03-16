#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/manager/file_table.h"

FTE *get_fte(PCB *aim, int fd);

/**
   Upon successful completion, lseek() returns the resulting offset  location as measured in bytes from the beginning of the file.
   On error, the value (off_t) -1 is returned
   TODO now lseek() not support the file offset go beyond the
   end of file but no check
 */
int lseek_file(Msg *m) {
    FTE *fte = get_fte((PCB *) m->buf, m->dev_id);
    int start = 0;
    int fsize = fte->filesize;
    switch (m->len) {
        case SEEK_CUR:
            start = fte->offset;
            break;
        case SEEK_SET:// start from beginning
            break;
        case SEEK_END: {
            start = fsize;
            break;
        }
        default:
            assert(0);
            break;
    }
    int res = m->offset + start;
    if (res > fsize || res < 0) {
        return -1;
    }
    fte->offset = res;

    m->ret = SUCC;
    return res;
}