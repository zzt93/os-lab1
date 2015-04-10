/* To understand macros here, see i386 manual. */
#define GDT_ENTRY(n)  \
	((n) << 3)

#define SEG_NULLASM   \
	.word 0, 0;       \
	.byte 0, 0, 0, 0

/* The 0xC0 means the limit is in 4096-byte units
   and (for executable segments) 32-bit mode. */
#define SEG_ASM(type,base,lim)                              \
	.word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
	.byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
	(0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)
