volatile int x = 0;
int main() {
	while(1) {
		if(x % 10000000 == 0) {
			asm volatile("movl $111, %eax; int $0x80");
		}

		x ++;
	}
	return 0;
}
