#ifndef __ERROR_H__
#define __ERROR_H__

extern const char *const err[];
extern const int len_err;
extern int err_size[];

#include "macro.h"

void init_error_msg();

#include "kernel/message.h"

#endif /* __ERROR_H__ */