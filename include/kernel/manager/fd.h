#ifndef __FD_H__
#define __FD_H__

#include "kernel/message.h"

typedef struct file_descriptor_entry {
    //int fd;
    void *ft_entry;
} FDE; // file descriptor entry

// invalid fd index
#define INVALID_FD_I -1

#define INVALID_FD NULL

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2


static inline int is_invalid_fd(FDE *fde) {
    return fde->ft_entry == NULL;
}

static inline void make_invalid(FDE *fde) {
    fde->ft_entry = NULL;
}

#include "file_table.h"

static inline int assign_fd(FDE *f, FDE *s) {
    f->ft_entry = s->ft_entry;
    FTE *aim = (FTE *)s->ft_entry;
    assert(aim != NULL);
    aim->ref_count ++;
    return 1;
}


int open_file(Msg *);
int close_file(Msg *);
int dup2_file(Msg *);
int dup_file(Msg *);
int lseek_file(Msg *);

#endif /* __FD_H__ */