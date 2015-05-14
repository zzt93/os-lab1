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

#endif /* __MESSAGE_H__ */