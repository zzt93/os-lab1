#include "common.h"
#include "lib/string.h"

#include "adt/int_stack.h"

#define HEXA_BI 4

void lock();

void unlock();

/**
   Lock or not???
 */

void printHexadecimal(void (*printer)(char), int);

/* implement this function to support printk */
void vfprintf(void (*printer)(char), const char *ctl, void **tmp) {
//	const char *str = __FUNCTION__;
//	for(;*str != '\0'; str ++) printer(*str);
//
//	str = ": vfprintf() is not implemented!\n";
//	for(;*str != '\0'; str ++) printer(*str);
    int *args = (int *) tmp;
    for (; *ctl != '\0'; ctl++) {
        if (*ctl != '%') {
            printer(*ctl);
        } else {
            /**
               why switch (cmp) then interrupt comes print
               go to the default option
             */
            //lock();
            char c = *(++ctl);
            //unlock();
            switch (c) {
                case 'x'://TODO suppose x is for int32
                {
                    printHexadecimal(printer, *args);
                    args++;
                    break;
                }
                case 'c':
                    printer(*args++);
                    break;
                case 'd': {
                    char *str = itoa(*args++);
                    print_str(printer, str);
                    break;
                }
                case 's': {
                    char **str = (char **) args;
                    args++;
                    print_str(printer, *str);
                    break;
                }
                default:
                    printer(c);
                    assert(0);
                    break;
            }
        }
    }
}

extern void serial_printc(char);

/* __attribute__((__noinline__))  here is to disable inlining for this function to avoid some optimization problems for gcc 4.7 */
/*
  如果一个函数的形参类型未知，例如使用了Old Style C风格的函数声明，或者函数的参数列表中有...，
  那么调用函数时要对相应的实参做Integer Promotion，此外，相应的实参如果是float型的也要被提升为double型，
  这条规则称为Default Argument Promotion。
  我们知道printf的参数列表中有...，除了第一个形参之外，其它形参的类型都是未知的，比如有这样的代码：

  char ch = 'A';
  printf("%c", ch); // ch要被提升为int型之后再传给printf。
*/
void __attribute__((__noinline__))
printk(const char *ctl, ...) {
    void **args = (void **) &ctl + 1;
    //lock();
    vfprintf(serial_printc, ctl, args);
    //unlock();
}

#include "drivers/tty/tty4.h"

void kprintf(const char *format, void **args) {
    lock();
    vfprintf(tty4_printc, format, args);
    flush();
    unlock();
}

void printHexadecimal(void (*printer)(char), int c) {
    int and = 0xf;
    printer('0');
    printer('x');
    unsigned int i;
    for (i = 0; i < (sizeof c) * 2; ++i) {
        int tmp = c & and;
        switch (tmp) {
            case 10:
                push('a');
                break;
            case 11:
                push('b');
                break;
            case 12:
                push('c');
                break;
            case 13:
                push('d');
                break;
            case 14:
                push('e');
                break;
            case 15:
                push('f');
                break;
            default:
                push(tmp + '0');
                break;
        }
        c = c >> HEXA_BI;
    }
    while (i > 0 && !empty()) {
        i--;
        printer(pop());
    }
}
