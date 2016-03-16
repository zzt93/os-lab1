#ifndef __FD_FT_H__
#define __FD_FT_H__

#include "fd.h"
#include "file_table.h"

int detach_fte(FDE *, FTE *);

static inline void assign_fte(FDE *d, FTE *t) {
    assert(d != NULL);
    if (t == NULL) {
        return;
    }
    if (!is_invalid_fd(d)) {
        detach_fte(d, d->ft_entry);
    }
    d->ft_entry = t;
    t->ref_count ++;
}

#endif /* __FD_FT_H__ */