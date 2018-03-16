#include "kernel/kernel.h"
#include "drivers/time.h"
#include "drivers/tty/tty.h"
#include "drivers/tty/term.h"
#include "kernel/message.h"

extern char banner[SCR_W + 1];

static char ani[] = "-\\|/";
static int tsc = 0;

static void
draw2(char **p, int q) {
    (*(*p)++) = q / 10 + '0';
    (*(*p)++) = q % 10 + '0';
}

void update_banner(void) {
    banner[1] = ani[tsc];
    tsc = (tsc + 1) % (sizeof(ani) - 1);
    char *p = banner + SCR_W - 20;
    Time tm;
    get_time(&tm);
    *p++ = '2';
    *p++ = '0';
    draw2(&p, tm.year % 100);
    *p++ = '/';
    draw2(&p, tm.month);
    *p++ = '/';
    draw2(&p, tm.day);
    *p++ = ' ';
    draw2(&p, tm.hour);
    *p++ = ':';
    draw2(&p, tm.minute);
    *p++ = ':';
    draw2(&p, tm.second);
    banner[3] = 't';
    banner[4] = 't';
    banner[5] = 'y';
    banner[6] = '1' + current_consl - ttys;
    consl_sync(current_consl);
}
