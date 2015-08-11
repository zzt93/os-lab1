#ifndef __FILE_TABLE_H__
#define __FILE_TABLE_H__

typedef enum {
    DEV,
    REG,
} Node_e;

typedef struct {
    void *node;
    int offset;
    int dev_id;
    int ref_count;
    Node_e type;
} FTE;

#define MAX_FILE 128

FTE * add_fte(iNode *node);
int free_fte(void *p);

#endif /* __FILE_TABLE_H__ */