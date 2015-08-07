#include "common.h"
#include "lib/string.h"
#include "adt/int_stack.h"

/* 注意！itoa只有一个缓冲，因此
 * char *p = itoa(100);
 * char *q = itoa(200);
 * 后p和q所指内容都是"200"。
 * itoa must write from end to start,
 * eg itoa(100);
 * itoa(20); if write from start, answer would be 200.

 */

/**
   lock or not:?
   lock&unlock may make the printk in NOINTR place invalid
   if not, buf might be changed
 */
char *itoa(int a) {
	static char buf[30];
	char *p = buf + sizeof(buf) - 1;
    if (a < 0) {
        do {
            *--p = '0' - a % 10;
        } while (a /= 10);
        *--p = '-';
    } else {
        do {
            *--p = '0' + a % 10;
        } while (a /= 10);
    }
	return p;
}

//TODO test
int itoa_s(int a, char *s, int limit) {
    int wid = 0;
    if (a < 0) {
        do {
            push('0' - a % 10);
        } while (a /= 10);
        push('-');
    } else {
        do {
            push('0' + a % 10);
        } while (a /= 10);
    }
    while(!empty()) {
        *s++ = pop();
        wid++;
    }
    return wid;
}
int itoh_s(int a, char *s, int limit) {
    return -1;
}

void memcpy(void *dest, const void *src, size_t size) {
	asm volatile ("cld; rep movsb" : : "c"(size), "S"(src), "D"(dest));
}

void memset(void *dest, uint8_t data, size_t size) {
	asm volatile ("cld; rep stosb" : : "c"(size), "a"(data), "D"(dest));
}

/**
   @NOTICE: Not include null character '\0'
   str = "abc" -- len = 3
 */
size_t strlen(const char *str) {
	int len = 0;
	while (*str ++) len ++;
	return len;
}

void strcpy(char *d, const char *s) {
	memcpy(d, s, strlen(s) + 1);
}

void print_str(void (*printer)(char), char *str) {
    while(*str != '\0') {
        printer(*str);
        str++;
    }
}

int strcmp(const char* f, const char* s) {
    int i = 0;
    while(f[i] == s[i] && f[i] != '\0') {
        i++;
    }
    return f[i] - s[i];
}

char to_upper(char c) {
    if (is_letter(c)) {
        return c & 0xdf;
    } else {
        printk(RED"Not a letter\n"RESET);
        return c;
    }
}

int is_letter(char c) {
    return (c <= 'z' && c >= 'a')
        || (c <= 'Z' && c >= 'A');
}
