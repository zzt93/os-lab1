#ifndef __STRING_H__
#define __STRING_H__

#include "common.h"

char *itoa(int);
int itoa_s(int a, char *buf, int limit);
void memcpy(void *, const void *, size_t);
void memset(void *, uint8_t, size_t);
size_t strlen(const char *);
void strcpy(char *d, const char *s);

int strcmp(const char* f, const char* s);
int is_letter(char);
char to_upper(char);

void print_str(void (*printer)(char), char *);

int to_int(char *);
int split(char *s, char delimiter, char **save);

#define INVALID_INDEX -1

int find_char(const char *str, int ith, char aim);
bool str_empty(const char * str);

#endif
