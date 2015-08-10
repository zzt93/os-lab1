#ifndef __FD_H__
#define __FD_H__

typedef struct {
    int fd;
    void *st_entry;
} FDE; // file descriptor entry

#define INVALID_FD -1

int open_file(Msg *);
int close_file(Msg *);

int make_invalid(PDE *);
int first_fd(PDE *start, int value);

#endif /* __FD_H__ */