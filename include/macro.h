#ifndef __MACRO_H__
#define __MACRO_H__


#define str(x) #x
#define conc(str1,str2) str1 ## str2

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define IF_MASK 0x200
#define INTR assert(read_eflags() & IF_MASK)
#define NOINTR assert(~read_eflags() & IF_MASK)

#define _ALIGN_(x, a) (((x) + (a)) & ~(a))

#endif /* __MACRO_H__ */
