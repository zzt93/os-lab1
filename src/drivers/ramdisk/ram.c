#include "drivers/ramdisk/ram.h"
#include "assert.h"
//#include "kernel/process.h"

static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE] = {
    {'h', 'e', 'l', 'l', 'o'},
    {'w', 'o'}
};
static uint8_t *disk = (void*)file;

void init_ram() {
    printk("%d ", disk[0]);
}