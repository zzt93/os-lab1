#ifndef __TIME_H__
#define __TIME_H__

#define HZ        100

typedef struct Time {
	int year, month, day;
	int hour, minute, second;
} Time;

enum {
    NEW_TIMER,
};

long get_jiffy();

void get_time(Time *tm);

extern int TIMER;

typedef struct {
    // TODO replace int with time_t?
    int time;
    int pid;
} Timer;

//void set_timer(Timer *t, int time, int pid);
// implemented in timer_sys.c

#include "kernel/message.h"

int kwait(Msg *m);
void update_timer();


int kstart_non_block_timer(int seconds);
int knon_block_timer_finished();
void update_non_block_timer();

// driver id of timer
extern int d_timer;

typedef enum {
    Seconds = 0x00,
    Minutes = 0x02,
    Hours = 0x04,
    Weekday = 0x06,
    Day_of_month = 0x07,
    Month = 0x08,
    Year = 0x09,
    Century = 0x32,
} TimeReg;


#endif
