#ifndef __FM_RAM_H__
#define __FM_RAM_H__

#include "common.h"

//void do_read(int file_name, uint8_t *buf, off_t offset, size_t len);

const uint8_t *const get_a_ram_file(int i, int *m_size);

#endif /* __FM_RAM_H__ */