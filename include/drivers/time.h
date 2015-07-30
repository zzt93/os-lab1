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

inline long get_jiffy();

void get_time(Time *tm);

extern int TIMER;

#endif
