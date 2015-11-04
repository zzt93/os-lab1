#ifndef __TTY4_H__
#define __TTY4_H__

#include "tty.h"

void tty4_printc(char);
void flush();

extern char * user_name;
extern Console * terminal;

#endif /* __TTY4_H__ */