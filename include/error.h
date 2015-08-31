#ifndef __ERROR_H__
#define __ERROR_H__

extern const char *const err[];
extern int err_size[];

#define ARR_LEN(c) (sizeof(x) / sizeof((x)[0]))

void init_error_msg();

#endif /* __ERROR_H__ */