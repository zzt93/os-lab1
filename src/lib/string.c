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

/**
 * If number is longer than buf_len, number will be truncate, but width
 * is accurate.
 * @param buf result buffer
 * @param buf_len
 * @return the width of this int
 */
int itoa_s(int a, char *buf, int buf_len) {
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
    int wid = 0;
    while (!empty()) {
        if (wid < buf_len - 1) {
            *buf++ = (char) pop();
        } else {
            pop();
        }
        wid++;
    }
    *buf = '\0';
    return wid;
}

int itoh_s(int a, char *s, int limit) {
    return -1;
}


void strcpy(char *d, const char *s) {
    memcpy(d, s, strlen(s) + 1);
}

void print_str(void (*printer)(char), char *str) {
    while (*str != '\0') {
        printer(*str);
        str++;
    }
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

/**
   return the index of `ith` `aim` in the string `str`
   if can't find suitable one, return a negative number
   ith -- 1 .default to be counted from left to right,
   2. can be negative, i.e. means count from right to left
   3. range of ith: [1, max] && [-max, -1]
 */
int find_char(const char *str, int ith, char aim) {
    int start = 0;
    if (ith < 0) {
        start = strlen(str);
        while (start--) {
            if (str[start] == aim) {
                ith++;
                if (ith == 0) {
                    return start;
                }
            }
        }
    } else if (ith > 0) {
        while (str[start] != '\0') {
            if (str[start] == aim) {
                ith--;
                if (ith == 0) {
                    return start;
                }
            }
            start++;
        }
    }
    return INVALID_INDEX;
}

bool str_empty(const char *str) {
    return *str == '\0';
}

/**
 * compare two string at most len bytes, if either string's size is smaller than len
 * stop at min {len(f), len(s)}
 * @param f
 * @param s
 * @param len
 * @return
 */
int strncmp(const char *f, const char *s, size_t len) {
    size_t i = 0;
    while (f[i] == s[i] && f[i] != '\0') {
        i++;
        if (i == len) {
            return 0;
        }
    }
    return f[i] - s[i];
}