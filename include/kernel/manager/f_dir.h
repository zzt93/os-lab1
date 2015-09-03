#ifndef __F_DIR_H__
#define __F_DIR_H__

#define MAX_FILENAME_LEN 28

typedef uint32_t inode_t;

typedef struct {
	char filename[MAX_FILENAME_LEN];
    // inode offset
	inode_t inode_off;
} Dir_entry;


#define MAX_DIR_DEPTH 16

extern const char *const current_dir;
extern const char *const father_dir;
extern const char *const default_cwd_name;

#include "error.h"

// the error must correspond to the order of err
// @see set_error_msg()
#define NO_SUCH 0
#define NOT_DIR 1

#include "inode.h"

uint32_t get_dir_e_off(iNode *dir, inode_t aim);

#endif /* __F_DIR_H__ */