#ifndef __F_DIR_H__
#define __F_DIR_H__

#define MAX_FILENAME_LEN 28

typedef int inode_t;

typedef struct {
	char filename[MAX_FILENAME_LEN];
    // inode offset
	inode_t inode_off;
} Dir_entry;


#define MAX_DIR_DEPTH 16

extern const char *const current_dir;
extern const char *const father_dir;

#define NOT_CONTAIN 0

#endif /* __F_DIR_H__ */