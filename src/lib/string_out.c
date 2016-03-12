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
   - This function is thread safe
   - it will change the delimiter in the string to '\0'
   - this function can't work for const string
   str -- point to string
   delimiter -- the delimiter char
   saveptr -- the pointer array store each part

   return: how many parts are splited
 */
int split(char *str, char delimiter, char **saveptr) {
    // skip heading delimiter
    while (*str != '\0' && *str == delimiter) {
        str++;
    }
    if (*str == '\0') {
        return 0;
    }

    // save the first part
    int count = 1;
    *saveptr ++ = str;
    char last_c = 0;
    while(*str != '\0') {
        if (*str != delimiter && last_c == delimiter) {
            *(str - 1) = '\0';
            count++;
            *saveptr ++ = str;
        }
        last_c = *str;
        str++;
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
	int len = 0;
	while (*str ++) len ++;
	return len;
}

int strcmp(const char* f, const char* s) {
    int i = 0;
    while(f[i] == s[i] && f[i] != '\0') {
        i++;
    }
    return f[i] - s[i];
}
