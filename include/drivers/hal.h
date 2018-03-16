#ifndef __HAL_H__
#define __HAL_H__

#include "common.h"
#include "adt/list.h"

enum {
    DEV_READ,
    DEV_WRITE,
    PROMPT,
};

#define MSG_HARD_INTR -2

void add_irq_handle(int, void (*)(void));

typedef struct Device {
    const char *name;
    pid_t pid;
    int dev_id;

    ListHead list;
} Dev;

size_t n_dev_read(int dev_id, pid_t reqst_pid, void *buf, off_t offset, size_t len);

size_t n_dev_write(int dev_id, pid_t reqst_pid, void *buf, off_t offset, size_t len);

void hal_register(const char *name, pid_t pid, int *dev_id);

//void hal_get_id(const char *name, pid_t *pid, int *dev_id);
void hal_list(void);

// devices' name
extern const char *hda;// pid
extern const char *timer;
extern const char *ram;

extern int d_ramdisk;
extern int d_ide;

#define hal_get_(aim, f) {                          \
        lock();                                     \
        ListHead *it;                               \
        list_foreach(it, &devices) {                \
            Dev *dev = list_entry(it, Dev, list);   \
            assert(dev);                            \
            if (f(dev, aim)) {                      \
                unlock();                           \
                return dev;                         \
            }                                       \
        }                                           \
        unlock();                                   \
        return NULL;                                \
    }                                               \


#include "time.h"
#include "tty/tty.h"

extern int now_disk;

#endif
