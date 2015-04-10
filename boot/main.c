/* start.S has put the processor into protected 32-bit mode,
	 and set up the right segmentation. The layout of our hard
	 disk is shown below:
	 +-----------+------------------.        .-----------------+
	 | bootblock |  The game binary    ...     (in ELF format) |
	 +-----------+------------------`        '-----------------+
	 So the task of the C code is to load the game binary into
	 correct memory location (0x100000), and jump to it. */

#include "boot.h"

#define SECTSIZE 512
#define KOFFSET  0xC0000000

void readseg(unsigned char *, int, int);

void
bootmain(void) {
	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;
	unsigned char* pa, *i;
	void (*entry)(void);
	unsigned int j;

	/* The binary is in ELF format (please search the Internet).
	   0x8000 is just a scratch address. Anywhere would be fine. */
	elf = (struct ELFHeader*)0x8000;

	/* Read the first 4096 bytes into memory.
	   The first several bytes is the ELF header. */
	readseg((unsigned char*)elf, 4096, 0);

	/* Load each program segment */
	ph = (struct ProgramHeader*)((char *)elf + elf->phoff);
	eph = ph + elf->phnum;
	for(; ph < eph; ph ++) {
		pa = (unsigned char*)(ph->paddr - KOFFSET); /* physical address */
		readseg(pa, ph->filesz, ph->off); /* load from disk */
		for (i = pa + ph->filesz; i < pa + ph->memsz; *i ++ = 0);
	}

	/* Here we go! */
	entry = (void(*)(void))(elf->entry - KOFFSET);
	entry(); /* never returns */
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* Spin on disk until ready */
}

/* Read a single sector (512B) from disk */
void
readsect(void *dst, int offset) {
	int i;
	/* Issue command */
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	/* Fetch data */
	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

/* Read "count" bytes at "offset" from binary into physical address "pa". */
void
readseg(unsigned char *pa, int count, int offset) {
	unsigned char *epa;
	epa = pa + count;
	pa -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 1;
	for(; pa < epa; pa += SECTSIZE, offset ++)
		readsect(pa, offset);
}
