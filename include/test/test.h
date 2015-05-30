#ifndef __TEST_H__
#define __TEST_H__

#include "test_create.h"
#include "test_malloc.h"
#include "test_queue.h"
#include "test_lock.h"

void test_list_entry();
void test_tree();
void test_map();

// implemented in util.c
void init_test_proc();

#endif /* __TEST_H__ */
