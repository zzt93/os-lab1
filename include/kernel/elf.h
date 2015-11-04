#ifndef __ELF_H__
#define __ELF_H__

/* Structure of a ELF binary header */
struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
    // the entry va of start function
	unsigned int   entry;
    // program header offset
	unsigned int   phoff;
    // section header offset
	unsigned int   shoff;
	unsigned int   flags;
    // size of this header
	unsigned short ehsize;
    // size of program header
	unsigned short phentsize;
    // program header number
	unsigned short phnum;
    // size of section header
	unsigned short shentsize;
    // section header number
	unsigned short shnum;
    // Section header string table index
	unsigned short shstrndx;
};

/* Structure of program header inside ELF binary */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
};



#endif /* __ELF_H__ */
