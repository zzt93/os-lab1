#include "common.h"

/**
   - This function is thread-safe
   - no invalid character check
   str -- just a plain decimal number, hex, octal is not allowed
 */
int to_int(char *str) {
    int res = 0;
    while (*str != '\0') {
        res *= 10;
        res += *str - '0';
        str++;
    }
    return res;
}

/**
    <li>this function is thread safe</li>
    <li>it will change the delimiter in the string to '\0'</li>
    <li>this function can't work for const string</li>
    <li>this function skip leading delimiter, but not trailing delimiter</li>

    @arg str -- point to string
    @arg delimiter -- the delimiter char
    @arg saveptr -- the pointer array store each part

    e.g.
    <li>"//" -- ["", ""]</li>
    <li>"/afd/asd/" -- ["", "afd", "asd", ""]</li>
    <li>"//sdf/sdf//" -- ["", "", "sdf", "sdf", "", ""]</li>
    <li>"asdf/sd//b" -- ["asdf", "sd", "", "b"]</li>


    @return: how many parts are splited
    tested by test_string.c#test_split
 */
int split(char * const str, char delimiter, char **saveptr) {
    if (*str == '\0') {
        return 0;
    }

    // save the first part
    int count = 0;
    char *s = str, *last_delimiter = str - 1;
    while (*s != '\0') {
        if (*s == delimiter) {
            *s = '\0';
            count++;
            *saveptr++ = last_delimiter + 1;
        }
        last_delimiter = s;
        s++;
    }
    return count;
}


void memset(void *dest, uint8_t data, size_t size) {
    asm volatile ("cld; rep stosb" : : "c"(size), "a"(data), "D"(dest));
}

void memcpy(void *dest, const void *src, size_t size) {
    asm volatile ("cld; rep movsb" : : "c"(size), "S"(src), "D"(dest));
}

/**
   @NOTICE: Not include null character '\0'
   str = "abc" -- len = 3
 */
size_t strlen(const char *str) {
    size_t len = 0;
    while (*str++) len++;
    return len;
}

int strcmp(const char *f, const char *s) {
    int i = 0;
    while (f[i] == s[i] && f[i] != '\0') {
        i++;
    }
    return f[i] - s[i];
}

void trim(char *const str, char trimChar) {
    char *f = str, *s = str;
    while (*s == trimChar && *s != '\0') {
        s++;
    }
    if (f != s) {
        while (*s != '\0') {
            *f++ = *s++;
        }
    }
}
