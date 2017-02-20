#ifndef __TYPES_H__
#define __TYPES_H__

/* USE THESE TYPES to specify integer varaiable size
   prefix "u" means unsigned, while suffix number indicates size */
typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef unsigned short uint16_t;
typedef          short int16_t;
typedef unsigned char  uint8_t;
typedef          char  int8_t;
typedef unsigned char  bool;

typedef          int   pid_t;
typedef          uint32_t   size_t;
typedef          int   off_t;

#define Pri_t int

typedef void * caddr_t;

typedef unsigned long u_long;

#endif
