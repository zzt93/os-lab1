#ifndef __TTY_H__
#define __TTY_H__

#include "kernel/message.h"

#define NR_TTY         4
#define MSG_TTY_GETKEY 1
#define MSG_TTY_UPDATE 2
#define LBUF_SZ        256
#define CBUF_SZ        1024
#define RSTK_SZ        16

typedef struct Console {
	const char *name;
    // the buffer in memory
	uint16_t *vbuf;
    // the memory mapped screen
	uint16_t *scr;
    // width, height, square
	int w, h, wh;
    // buffer size and position of cursor
	int size, pos;
    // line buffer
	char lbuf[LBUF_SZ + 1];
    // cooked buffer -- all the user input except the new line
	char cbuf[CBUF_SZ + 1];
    // r -- the index for cooked buffer
    // f -- the index for already handled char in cooked buf
    // i -- the index for line buffer
	int i, f, r;
    // the index for message stack?
	int rtop;
    // the request message stack
	Msg rstk[RSTK_SZ];
} Console;

extern pid_t TTY;
extern Console ttys[];
extern Console *current_consl;

void consl_sync(Console *c);
void consl_writec(Console *c, char ch);
void consl_feed(Console *c, int key);
void consl_accept(Console *c, char ch);
void readkey(void);
void update_banner(void);
void read_request(Msg *m);
size_t handle_write_request(Msg *m);

int put_prompt(Msg *m);

#define TTY4 "tty4"
// the now main output/input terminal index
// now is last tty
#define NOW_TERMINAL (NR_TTY-1)
// device id
extern int d_ttyi[];
extern int tty_start;

#endif
