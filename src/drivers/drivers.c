#include "drivers/hal.h"

void init_hal();
void init_timer();
void init_tty();
void init_ide();

void init_driver() {
	init_hal();
	init_timer();
	init_tty();
	init_ide();

	hal_list();
}
