#include "kernel/kernel.h"
#include "x86/x86.h"
#include "drivers/tty/tty.h"
#include "lib/string.h"
#include "drivers/tty/term.h"
#include "drivers/time.h"
#include "drivers/hal.h"

#include "kernel/manager/MM_util.h"

Console ttys[NR_TTY];
Console *current_consl;


static const char *ttynames[NR_TTY] = {"tty1", "tty2", "tty3", TTY4};

// for tty4
//Console *terminal = ttys + NR_TTY - 1;
extern char *user_name;

// real memory of screen
// memory-mapped io: screen
static uint16_t *vmem = (void *) pa_to_va(0xb8000);

static uint16_t vbuf[NR_TTY][SCR_W * SCR_H * 2];

char banner[SCR_W + 1];


const char *
get_current_tty(void) {
    return current_consl->name;
}

static uint16_t
draw(char ch) {
    return (C_BLACK << 12) | (C_WHITE << 8) | ch;
}

static void
scrup(Console *c) {
    int i;
    if (c->scr + c->wh >= c->vbuf + c->size) {
        for (i = c->w; i < c->size; i++)
            c->vbuf[i - c->w] = c->vbuf[i];
        c->scr -= c->w;
        c->pos -= c->w;
    }
    for (i = 0; i < c->w; i++)
        c->scr[c->wh + i] = draw(' ');
    c->scr += c->w;
}

// move the cursor next
static void
next(Console *c) {
    if (c->vbuf + c->pos == c->scr + c->wh - 1) {
        scrup(c);
    }
    c->pos++;
}

// put char on now position
static void
putc(Console *c, char ch) {
    *(c->vbuf + c->pos) = draw(ch);
}

/**
   make pos back to the begin of this line
 */
static void
cr(Console *c) {
    c->pos -= c->pos % c->w;
}

/**
   add a line to position
 */
static void
lf(Console *c) {
    c->pos += c->w;
    if (c->pos >= c->scr - c->vbuf + c->wh) {
        scrup(c);
    }
}

static void
del(Console *c) {
    c->pos--;
    putc(c, ' ');
}

/**
   update index of current input -- i
   and the cursor -- pos
 */
static bool
movl(Console *c) {
    if (c->i != 0) {
        c->i--;
        c->pos--;
        return true;
    }
    return false;
}

static bool
movr(Console *c) {
    if (c->lbuf[c->i] != 0) {
        c->i++;
        c->pos++;
        return true;
    }
    return false;
}

/**
   synchronize the content in memory and screen
   write to memory mapped io -- screen
   port mapped io -- for what?
 */
void
consl_sync(Console *c) {
    int i;
    lock();
    // write banner
    for (i = 0; i < SCR_W; i++) {
        vmem[i] = (C_BLUE << 12) | (C_LWHITE << 8) | banner[i];
    }
    if (current_consl == c) {
        memcpy(vmem + c->w, c->scr, c->wh * 2);
        // draw corsor then clear it
        int pos = c->pos - (c->scr - c->vbuf) + c->w;
        out_byte(0x3d4, 0xe);
        out_byte(0x3d5, pos >> 8);
        out_byte(0x3d4, 0xf);
        out_byte(0x3d5, pos & 0xff);
    }
    unlock();
}

static void
removec(Console *c) {
    char *ptr = &c->lbuf[--c->i];
    uint16_t *scr = &c->vbuf[c->pos - 1];
    while (*ptr != 0) {// move late chars back when delete a char in
        // the middle of input array
        *ptr = *(ptr + 1);
        *scr = *(scr + 1);
        ptr++;
        scr++;
    }
}

#include "drivers/sound.h"

static uint8_t
start_sound() {
    uint8_t state = in_byte(PORT_PC_SPEAKER) & 0xFC;
    out_byte(PORT_PC_SPEAKER, state | PC_SPEAKER_ON);
    return state;
}

static void
close_sound(uint8_t state) {
    out_byte(PORT_PC_SPEAKER, state);
}

void
pit_sound(uint16_t freq) {
    uint16_t counter = PIT_FREQ / freq;
    out_byte(PIT_CONTROL, 0xB6);
    out_byte(PIT_CHANNEL_2, counter & 0x0f);
    out_byte(PIT_CHANNEL_2, counter >> 8);
    out_byte(PORT_PC_SPEAKER, in_byte(PORT_PC_SPEAKER) | 0x3);
}

void
pit_stop() {
    out_byte(PORT_PC_SPEAKER, in_byte(PORT_PC_SPEAKER) & 0xFC);
}

static void
wait_sometime() {
    volatile int count = 0;
    while (count < 10000) {
        count++;
    }
    // can't stop this thread, will stop screen update
//   Msg m;
//   m.i[0] = 1;
//   m.i[1] = current->pid;
//   kwait(&m);
//   receive(TIMER, &m);
}

#include "drivers/time.h"

static void
backsp(Console *c) {
    if (c->i > 0) {
        removec(c);
        if (c->lbuf[c->i] == 0) {
            del(c);
        } else {
            c->pos--;
        }
    } else {// c->i == 0, i.e. at the start of the line buffer
        /* Insert code here to play some warning sound */
        /* may change to rather thread to make sound, rather than
           blocking TTY
         */
        assert(current->pid == TTY);
        volatile int count = 0;
        while (count < 10000) {
            uint8_t s = start_sound();
            wait_sometime();
            close_sound(s);
            count++;
        }
    }
}

/**
   copy a newly-added line to buffer
   update the index of resolved cooked buffer -- f
 */
static size_t
get_cooked(Console *c, pid_t pid, char *buf, int count) {
    assert(c->f != c->r);
    assert(c->r > c->f);
    // move it to dev_rw -- move it is wrong
    // convertion have to be in the target process and
    // dev_rw is at source process
    char *aim = get_pa(&fetch_pcb(pid)->pdir, (uint32_t) buf);
    int nread = 0;
    while (count--) {
        if (c->cbuf[c->f] == 0) {// out of range of cooked buffer
            //c->f = (c->f + 1) % CBUF_SZ;
            break;
        }
        memcpy(aim++, c->cbuf + c->f, 1);
        nread++;
        c->f = (c->f + 1) % CBUF_SZ;
    }
    return nread;
}

/**
   only handle other devices read request when an enter is pressed
   ie, this line can't be changed
   pressing [key] -- read_key -- consl_feed -> cook -> read_request
   ask for read -- dev_read ->dev_rw -> send to tty -> ttyd -> read_request
 */
void
read_request(Msg *m) {
    Console *c = &ttys[m->dev_id - tty_start];
    if (c->f == c->r) {
        if (c->rtop == RSTK_SZ) panic("too many read request");
        // just copy the message when no more input in cooked buffer
        memcpy(&c->rstk[c->rtop++], m, sizeof(Msg));
    } else {// get newly added a line(end with an enter)
        pid_t dest = m->src;
        int nread = get_cooked(c, m->req_pid, m->buf, m->len);
        m->ret = nread;
        m->src = TTY;
        send(dest, m);
    }
}

size_t handle_write_request(Msg *m) {
    size_t i;
    if (m->dev_id >= tty_start && m->dev_id < NR_TTY + tty_start) {
        PCB *req_pcb = fetch_pcb(m->req_pid);
        assert(req_pcb != NULL);
        char *src = get_pa(&(req_pcb->pdir), (uint32_t) m->buf);
        // copy from the message buffer one by one
        for (i = 0; i < m->len; i++) {
            // FIXED: m->buf need changing to physical address
            consl_writec(&ttys[m->dev_id - tty_start], *(src + i));
        }
        consl_sync(&ttys[m->dev_id]);
    } else {
        assert(0);
    }
    return i;
}


int put_prompt(Msg *m) {
    m->dev_id = d_ttyi[NOW_TERMINAL];
    m->req_pid = current->pid;
    PCB *req_pcb = (PCB *) m->buf;
    char *str = user_name;
    int len1 = strlen(str);
    int len2 = strlen(req_pcb->cwd_path);
    // add 3 for "$ \0"
    m->len = len1 + len2 + 3;
    char prompt[m->len];
    memcpy(prompt, str, len1);
    memcpy(prompt + len1, req_pcb->cwd_path, len2);
    prompt[len1 + len2] = '$';
    prompt[len1 + len2 + 1] = ' ';
    prompt[len1 + len2 + 2] = '\0';
    m->buf = prompt;
    return handle_write_request(m);
}


/**
   to emulate the behavior of pressing [enter]
   :update the index of to be handled character in
   cooked buffer -- flush the line input
 */
static void
cook(Console *c) {
    //printk("Capture: %s\n", c->lbuf);
    char *p = c->lbuf;
    while (*p != '\0') {
        // copy the content in line buf to cooked buffer
        c->cbuf[c->r] = *p;
        // calculate the next valid index
        c->r = (c->r + 1) % CBUF_SZ;
        if (c->r == c->f) {
            panic("cooked buffer full");
        }
        p++;
    }
    // set the line buffer clear by set the first char to '\0'
    c->lbuf[c->i = 0] = 0;
    cr(c);
    lf(c);
    if (c->rtop != 0) {
        c->rtop--;
        read_request(&c->rstk[c->rtop]);
    }
    //put_user_name(c);
}

void
consl_writec(Console *c, char ch) {
    switch (ch) {
        case '\r':
            cr(c);
            break;
        case '\n':// print a '\n' just change of cursor position
            cr(c);
            lf(c);
            break;
        default:
            putc(c, ch);
            next(c);
    }
}

void
consl_accept(Console *c, char ch) {
    int i, cc = 0;
    // find the first empty char
    for (; c->lbuf[c->i + cc] != 0; cc++);
    for (i = cc + 1; i > 0; i--) {
        if (c->i + i >= LBUF_SZ) panic("line buffer full");
        // for insert -- move the content after i one place
        c->lbuf[c->i + i] = c->lbuf[c->i + i - 1];
        if (c->vbuf + c->pos + i == c->scr + c->wh + 1) {
            scrup(c);
        }
        c->vbuf[c->pos + i] = c->vbuf[c->pos + i - 1];
    }
    putc(c, ch);
    next(c);
    if (c->i >= LBUF_SZ) panic("line buffer full");
    // put the char in the line buffer
    c->lbuf[c->i++] = ch;
    consl_sync(c);
}

void
consl_feed(Console *c, int key) {
    switch (key) {
        case K_ENTR:
            cook(c);
            break;
        case K_DEL:
        case K_BACK:
            if (key == K_DEL) movr(c);
            backsp(c);
            break;
        case K_LEFT:
            movl(c);
            break;
        case K_RIGHT:
            movr(c);
            break;
        case K_HOME:
            while (movl(c));
            break;
        case K_END:
            while (movr(c));
            break;
        case K_F1:
        case K_F2:
        case K_F3:
        case K_F4:
        case K_F5:
        case K_F6:
            if (key - K_F1 < NR_TTY) {
                c = current_consl = ttys + key - K_F1;
            }
            break;
    }
    consl_sync(c);
}

static void
init_consl(int tty_index) {
    Console *c = ttys + tty_index;
    c->name = ttynames[tty_index];
    c->vbuf = vbuf[tty_index];
    c->scr = vbuf[tty_index];
    c->size = SCR_W * SCR_H * 2;
    c->pos = 0;
    c->w = SCR_W;
    c->h = SCR_H;
    c->wh = c->w * c->h;
    c->lbuf[0] = 0;

    int i;
    for (i = 0; i < SCR_W * SCR_H; i++) {
        c->scr[i] = draw(' ');
    }
    c->i = c->r = c->f = 0;
    c->rtop = 0;
    //put_user_name(c);
    consl_sync(c);
}

/**
   notice TTY to update the time on the screen
 */
static void
send_updatemsg(void) {
    if (get_jiffy() % (HZ / 10) == 0) {
        static Msg m;
        m.src = MSG_HARD_INTR;
        m.type = MSG_TTY_UPDATE;
        send(TTY, &m);
    }
}

int d_ttyi[NR_TTY];
int tty_start;

/**
   init banner on the screen
   init console for each tty_
   register each tty_
 */
void init_console(void) {
    memset(banner, ' ', sizeof(banner));
    banner[SCR_W] = 0;
    int i;
    for (i = 0; i < NR_TTY; i++) {
        init_consl(i);
        // TTY? it is the device pid, and we send to main TTY thread
        // register the pid of drivers thread
        hal_register(ttys[i].name, TTY, d_ttyi + i);
    }
    tty_start = d_ttyi[0];
    current_consl = ttys;
    // register time change on the screen and cursor of screen handler
    add_irq_handle(0, send_updatemsg);
}
