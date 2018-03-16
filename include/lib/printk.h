#ifndef __PRINTK_H__
#define __PRINTK_H__

void printk(const char *ctl, ...);

void kprintf(const char *foramt, void **args);

#endif /* __PRINTK_H__ */