#ifndef __FD_H__
#define __FD_H__

typedef struct {
    //int fd;
    void *ft_entry;
} FDE; // file descriptor entry

#define INVALID_FD -1

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int open_file(Msg *);
int close_file(Msg *);

static inline int is_invalid_fd(FDE *fde) {
    return fde->ft_entry == NULL;
}

static inline int make_invalid(FDE *fde) {
    fde->ft_entry = NULL;
}

int first_fd(FDE *start, int value);

#endif /* __FD_H__ */