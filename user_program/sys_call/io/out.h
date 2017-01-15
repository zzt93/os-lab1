#ifndef __OUT_H__
#define __OUT_H__

#include "c_lib.h"

int puts(const char *ctl);

int printf(const char *format, ...);

#define u_assert(cond, fmt, ...)                                               \
    do {                                                                \
        if(!(cond)) {                                                   \
            printf(                                                     \
                "assertion fail (%s) in function \"%s\", line %d, file \"%s\"\n", \
                #cond, __FUNCTION__, __LINE__, __FILE__);               \
            user_assert(0, fmt, __VA_ARGS__);                                                     \
        }                                                               \
    } while(0)


#endif /* __OUT_H__ */
