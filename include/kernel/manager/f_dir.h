#ifndef __F_DIR_H__
#define __F_DIR_H__

typedef int inode_t;

struct dir_entry {
	char filename[32];
    // inode offset
	inode_t inode_off;
};

#endif /* __F_DIR_H__ */