#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "types.h"
#include "adt/list.h"

typedef struct Message {
	pid_t src, dest;
	union {
		int type;
		int ret;
	};
	union {
		int i[5];
		struct {
			pid_t req_pid;
			int dev_id;
			void *buf;
			off_t offset;
			size_t len;
		};
	};
	ListHead list;
} Msg;

void send(pid_t dest, Msg *m);
void receive(pid_t src, Msg *m);

void init_msg(
    Msg* m,
    pid_t src,
    int type_return,
    pid_t r, int d, void *b, off_t o, size_t l);

#define INVALID_ID -1
//#define FAIL 0
//#define SUCC 1

// the following is a internal error message,
// should not put it in the following enum
#define NOT_DIR 1

// the error must correspond to the order of err in error.c
// e.g. err[NO_SUCH] == "No such file or directory"
// @see set_error_msg(), error.c
typedef enum {
    // MM related error
    MM_ERR = -14,
    NO_MORE_MEMORY,
    // PM related error
    PM_ERR,
    NOT_EXE,
    INVALID_PRI,
    SEGMENTATION,
    // FM related error
    FM_ERR,
    INVALID_FILENAME,
    BUF_OF,
    NO_MORE_DISK,
    FILE_EXIST,
    IS_DIR,
    // no such directory or file
    NO_SUCH,
    FAIL,
    SUCC,
} Msg_res;

/**
   In order to return more useful information
   if succ: return what this function have
   if fail: return failure code -- in m.ret
 */
#define SET_IF_SUCC(m, info) {                  \
        if (m.ret == SUCC) {                    \
            m.ret = info;                       \
        }                                       \
    }                                           \

static inline
int has_error(int msg_res) {
    return msg_res < 0;
}

#endif /* __MESSAGE_H__ */
