#include "adt/bit_map.h"

BIT_MAP(MAX_FILE)

static FTE file_table[MAX_FILE];
static int index;

void fill_fte(FTE *fte, iNode *node) {
    fte->node = node;
    fte->offset = 0;
    fte->dev_id = node->dev_id;
    fte->ref_count = 0;
    // TODO pointing an inode means not device?
    fte->type = REG;
}

FTE * add_fte(iNode *node) {
    // find the first free
    int i = first_val(FREE);
    // initialize fte
    FTE *aim = file_table + i;
    fill_fte(fte, node);
    set_val(i, USED);
    return aim;
}

int free_fte(void *p) {
    assert(p != NULL);
    FTE *p = (FTE *)p;
    lock();
    set_val(p - file_table, FREE);
    unlock();
    return 1;
}