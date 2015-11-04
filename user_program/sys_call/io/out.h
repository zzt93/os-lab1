#ifndef __OUT_H__
#define __OUT_H__

//#include "kernel/syscall.h"

int puts(const char *ctl);
int printf(const char *format, ...);

#define user_assert(cond) do { \
	if(!(cond)) { \
		printf("assertion fail (%s) in function \"%s\", line %d, file \"%s\"\n", \
	#cond, __FUNCTION__, __LINE__, __FILE__); \
	} \
} while(0)


#endif /* __OUT_H__ */
