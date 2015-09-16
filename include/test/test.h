#ifndef __TEST_H__
#define __TEST_H__

#include "test_create.h"
#include "test_malloc.h"
#include "test_queue.h"
#include "test_lock.h"
#include "test_file.h"

void test_list_entry();
void test_tree();
void test_map();
void test_bit_map();
void test_d_bit_map();
void test_heap();

void test_string();

void test_page_alloc();

// implemented in util.c
void init_test_proc();

void test_random();

#endif /* __TEST_H__ */
