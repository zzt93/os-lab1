#include "common.h"
#include "x86/x86.h"
#include "kernel/memory.h"

#include "lib/malloc.h"
#include "test/test_malloc.h"

void init_page(void);
void init_serial(void);
void init_segment(void);
void init_idt(void);
void init_intr(void);
void init_proc(void);
void welcome(void);

void os_init_cont(void);

void
os_init(void) {
	/* Notice that when we are here, IF is always 0 (see bootloader) */

	/* We must set up kernel virtual memory first because our kernel
	   thinks it is located in 0xC0000000.
	   Before setting up correct paging, no global variable can be used. */
	init_page();

	/* After paging is enabled, we can jump to the high address to keep
	 * consistent with virtual memory, although it is not necessary. */
	asm volatile (" addl %0, %%esp\n\t\
					jmp *%1": : "r"(KOFFSET), "r"(os_init_cont));

	assert(0);	// should not reach here
}

void
os_init_cont(void) {
	/* Reset the GDT. Although user processes in Nanos run in Ring 0,
	   they have their own virtual address space. Therefore, the
	   old GDT located in physical address 0x7C00 cannot be used again. */
	init_segment();

	/* Initialize the serial port. After that, you can use printk() */
	init_serial();

	/* Set up interrupt and exception handlers,
	   just as we did in the game. */
	init_idt();

	/* Initialize the intel 8259 PIC. */
	init_intr();

	/* Initialize processes. You should fill this. */
	init_proc();

    // initialize kmalloc
    init_kmalloc();
	welcome();

	sti(); // set interrupt enabled

	/* This context now becomes the idle process. */
	while (1) {
		wait_intr();
	}
}

void
welcome(void) {
	printk("Hello, OS World!\n");
    /*
    printk("Printk test begin...\n");
    printk("the answer should be:\n");
    printk("#######################################################\n");
    printk("Hello, welcome to OSlab! I'm the body of the game. ");
    printk("Bootblock loads me to the memory position of 0x100000, and Makefile also tells me that I'm at the location of 0x100000. ");
    printk("~!@#$^&*()_+`1234567890-=...... ");
    printk("Now I will test your printk: ");
    printk("1 + 1 = 2, 123 * 456 = 56088\n0, -1, -2147483648, -1412505855, -32768, 102030\n0, ffffffff, 80000000, abcdef01, ffff8000, 18e8e\n");
    printk("#######################################################\n");
    printk("your answer:\n");
    printk("=======================================================\n");
    printk("%s %s%scome %co%s", "Hello,", "", "wel", 't', " ");
    printk("%c%c%c%c%c! ", 'O', 'S', 'l', 'a', 'b');
    printk("I'm the %s of %s. %s 0x%x, %s 0x%x. ", "body", "the game", "Bootblock loads me to the memory position of",
        0x100000, "and Makefile also tells me that I'm at the location of", 0x100000);
    printk("~!@#$^&*()_+`1234567890-=...... ");
    printk("Now I will test your printk: ");
    printk("%d + %d = %d, %d * %d = %d\n", 1, 1, 1 + 1, 123, 456, 123 * 456);
    printk("%d, %d, %d, %d, %d, %d\n", 0, 0xffffffff, 0x80000000, 0xabcedf01, -32768, 102030);
    printk("%x, %x, %x, %x, %x, %x\n", 0, 0xffffffff, 0x80000000, 0xabcedf01, -32768, 102030);
    printk("=======================================================\n");
    printk("Test end!!! Good luck!!!\n");
    */
    test_malloc_free();
    test_free_in_order();
    test_free_random();
    test_malloc_free_large();
    test_malloc();
}
