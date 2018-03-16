#ifndef __RAM_H__
#define __RAM_H__

#define NR_MAX_FILE 8
#define NR_FILE_SIZE (128 * 1024)

#include "kernel/message.h"

void init_ram();

int read_ram(Msg *);

#endif /* __RAM_H__ */
