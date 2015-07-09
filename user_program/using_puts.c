#pragma GCC push_options
#pragma GCC optimize ("align-functions=1")

#include "sys_call/io/out.h"

volatile int x = 0;

int main() {
	while(1) {
		if(x % 10000000 == 0) {
			puts("Hello World!\n");
		}

		x ++;
	}
	return 0;
}

#pragma GCC push_options
