#ifndef __ASSERT_H__
#define __ASSERT_H__

#include "x86/x86.h"
#include "common.h"

/* system panic */
#define panic(...) do { \
	cli(); \
	printk("\n\33[1;31msystem panic\33[0m in function \"%s\", line %d, file \"%s\":\n", \
		__FUNCTION__, __LINE__, __FILE__); \
	printk(__VA_ARGS__); \
	while(1) wait_intr(); \
} while(0)


#define assert(cond) do { \
	if(!(cond)) { \
		panic("assertion fail (%s) in function \"%s\", line %d, file \"%s\"\n", \
	#cond, __FUNCTION__, __LINE__, __FILE__); \
	} \
} while(0)

#endif
