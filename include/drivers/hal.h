#ifndef __HAL_H__
#define __HAL_H__

#include "common.h"
#include "adt/list.h"

#define DEV_READ 1
#define DEV_WRITE 2

#define MSG_HARD_INTR -2

void add_irq_handle(int, void (*)(void));

typedef struct Device {
	const char *name;
	pid_t pid;
	int dev_id;

	ListHead list;
} Dev;

size_t dev_read(const char *dev_name, pid_t reqst_pid, void *buf ,off_t offset, size_t len);
size_t dev_write(const char *dev_name, pid_t reqst_pid, void *buf ,off_t offset, size_t len);

void hal_register(const char *name, pid_t pid, int dev_id);
void hal_get_id(const char *name, pid_t *pid, int *dev_id);
void hal_list(void);

#endif
