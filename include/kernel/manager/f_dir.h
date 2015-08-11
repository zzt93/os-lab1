#ifndef __F_DIR_H__
#define __F_DIR_H__

typedef int inode_t;

struct dir_entry {
	char filename[32];
    // inode index
	inode_t inode;
};

#endif /* __F_DIR_H__ */