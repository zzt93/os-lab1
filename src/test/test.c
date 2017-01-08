//
// Created by zzt on 11/16/16.
//

#include "test/test.h"
#include "const.h"

void init_base_dir_for_test();

void test_printk() {
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
}

void test() {
//    test_printk();
    /*
    test_malloc_free();
    test_free_in_order();
    test_free_random();
    test_malloc_free_large();
    test_malloc();
    */
    /*
    test_queue();
    test_queue_capacity();
    */
    /*
    test_list_entry();
    */
    /*
    test_tree();
    test_page_alloc();
    test_bit_map();
    test_random();

    test_d_bit_map();
    test_map();
    test_LCM();
    test_nb_timer();
    test_heap();
    */
    test_string();
    init_base_dir_for_test();
    test_simplify_path();
    read_ram_write_disk();
}

void init_base_dir_for_test() {
    test_list(NULL);
    test_mkdir();
    //test_deldir();
    test_ch();
    //test_mk_del();
    test_create();
    //test_delfile();
    //test_create_del();
    test_open_close();
    test_read_write();
}
