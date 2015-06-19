#include "lib/out.h"

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
