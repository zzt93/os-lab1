#include "kernel/kernel.h"
#include "drivers/hal.h"
#include "x86/x86.h"
#include "drivers/tty/tty.h"
#include "drivers/tty/term.h"

void
send_keymsg(void) {
	Msg m;
	m.type = MSG_TTY_GETKEY;
	m.src = MSG_HARD_INTR;
	send(TTY, &m);
}

static int caps, ctrl, alt, shft;

static int keychar[2][128] = {
	{   0 ,   0 ,  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',
	   '9',  '0',  '-',  '=',   0 ,   0 ,  'q',  'w',  'e',  'r',
	   't',  'y',  'u',  'i',  'o',  'p',  '[',  ']',   0,    0 ,
	   'a',  's',  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
	  '\'',  '`',   0 , '\\',  'z',  'x',  'c',  'v',  'b',  'n',
	   'm',  ',',  '.',  '/',   0 ,   0 ,   0 ,  ' '},
	{   0 ,   0 ,  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',
	   '(',  ')',  '_',  '+',   0 ,   0 ,  'Q',  'W',  'E',  'R',
	   'T',  'Y',  'U',  'I',  'O',  'P',  '{',  '}',   0,    0 ,
	   'A',  'S',  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
	   '"',  '~',   0 ,  '|',  'Z',  'X',  'C',  'V',  'B',  'N',
	   'M',  '<',  '>',  '?',   0 ,   0 ,   0 ,  ' '},
};

void
readkey(void) {
	uint32_t code = in_byte(0x60);
	in_byte(0x61);

	if (code >= 128) {
		code -= 128;
		switch (code) {
			case K_LSHFT: shft --; break;
			case K_RSHFT: shft --; break;
			case K_CTRL: ctrl --; break;
			case K_ALT: alt --; break;
			case K_CAPS: caps &= 1; break;
		}
	} else {
		int c = keychar[0][code];
		if (c >= 'a' && c <= 'z') {
			c = keychar[(shft & 1) ^ (caps & 1)][code];
		} else {
			c = keychar[shft & 1][code];
		}
		if (c != 0) {
			consl_accept(current_consl, c);
		}
		switch (code) {
			case K_ENTR: 
			case K_BACK:
			case K_LEFT:
			case K_RIGHT:
			case K_HOME:
			case K_END:
			case K_DEL:
			case K_F1:
			case K_F2:
			case K_F3:
			case K_F4:
			case K_F5:
			case K_F6:
				consl_feed(current_consl, code); break;
			case K_F12:
				printk("\33[1;31mWill now reboot.\33[0m\n");
				asm volatile("movl $0, %esp; pushl $0");
				assert(0);
				break;
			case K_LSHFT: shft ++; break;
			case K_RSHFT: shft ++; break;
			case K_CTRL: ctrl ++; break;
			case K_ALT: alt ++; break;
			case K_CAPS: caps ++; break;
		}
	}
}
