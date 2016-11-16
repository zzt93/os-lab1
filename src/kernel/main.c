#include "common.h"
#include "error.h"
#include "x86/x86.h"
#include "kernel/memory.h"

#include "lib/malloc.h"
#include "lib/string.h"
#include "kernel/semaphore.h"
#include "test/test.h"
#include "kernel/manager/manager.h"

void init_page(void);

void init_serial(void);

void init_segment(void);

void init_idt(void);

void init_intr(void);

void init_idle(void);

void init_driver();

void welcome(void);

void os_init_cont(void);

void more_frequent();


void
os_init(void) {
    /* Notice that when we are here, IF is always 0 (see bootloader) */

    /* We must set up kernel virtual memory first because our kernel
       thinks it is located in 0xC0100000.
       Before setting up correct paging, no global variable can be used. */
    // Before it, the address is physical address
    init_page();

    /* After paging is enabled, we can jump to the high address to keep
     * consistent with virtual memory, although it is not necessary. */
    asm volatile (" addl %0, %%esp\n\t\
                    jmp *%1": : "r"(KOFFSET), "r"(os_init_cont));

    assert(0);    // should not reach here
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
    //The Intel 8259 is a Programmable Interrupt Controller (PIC)
    init_intr();

    /**
       initialize kmalloc -- have to initialize it before init
       process, for using it in allocating memory for PCB
    */
    init_kmalloc();
    // make it NOINTR, can receive msg, can schedule
    init_idle();

    NOINTR;

    /**
       init_driver() have to before init_file_system() for FM have
       to send message to `ide` to read file system
     */
    init_driver();
    init_manager();
    NOINTR;
    init_error_msg();
    // init_proc() and init_manager() can replace??
    // solved by split set count_of_lock out of init_proc();

    //more_frequent();
    // init empty thread
    init_proc_test();

    // here is to initialize shell process, which must later
    // than init_manager -- for it will send message to
    // managers
    //ram_user_process();


    // @checked: move from locked state to unlocked state
    init_file_system();

    unlock(); // set interrupt enabled
    INTR;

    /**
       init_file_system() have to before init_proc()
       for init_proc() will using the `default_cwd` which is
       initialized by FM
    */
    /* Initialize the state of process idle, ie the running
       process for set up right lock num to avoid other
       initialization enable the interrupt and cause problem
    */

    // @checked: move from locked state to unlocked state
    welcome();

    user_process();
    // set idle not to using cpu time
    // for it is originally set to sleep when send message
    current->state = SLEEPED;


    /* This context now becomes the idle process. */
    while (1) {
        printk("!");
        wait_intr();
    }
}

void
welcome(void) {
    printk("Hello, OS World!\n");

    test();
}

/**
 * Using the idle thread to create some files and folders,
   make idle change state to slept when waiting for a
   message, may cause some bugs.
 */

void init_error_msg() {
    int i;
    for (i = 0; i < len_err; i++) {
        err_size[i] = strlen(err[i]) + 1;
    }
}

/*
  have to invoke before the first unlock
  used to make timer interrupt more frequent
*/
void more_frequent() {
#define PORT_TIME 0x40
#define FREQ_8253 1193182
#define HZ        100000

    int count = FREQ_8253 / HZ;
    assert(count < 65536);
    out_byte(PORT_TIME + 3, 0x34);
    out_byte(PORT_TIME, count % 256);
    out_byte(PORT_TIME, count / 256);
}