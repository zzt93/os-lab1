#ifndef __STRING_H__
#define __STRING_H__

#include "common.h"

char *itoa(int);

int itoa_s(int a, char *buf, int buf_len);

void memcpy(void *dest, const void *src, size_t);

void memset(void *dest, uint8_t data, size_t);

size_t strlen(const char *s);

void strcpy(char *d, const char *s);

int strcmp(const char *f, const char *s);

int strncmp(const char *f, const char *s, size_t len);

int is_letter(char);

char to_upper(char);

void print_str(void (*printer)(char), char *);

int to_int(char *);

int split(char *s, char delimiter, char **save);

#define INVALID_INDEX -1

int find_char(const char *str, int ith, char aim);

bool str_empty(const char *str);

void ltrim(char *const str, char trimChar);

void trim(char *const str, char trimChar);

#endif
