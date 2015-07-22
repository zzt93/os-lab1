#ifndef __MM_UTIL_H__
#define __MM_UTIL_H__

uint32_t get_pa(CR3 *pdir, uint32_t va);

#define page_frame_to_address(t) \
    (t->page_frame << 12)

#endif /* __MM_UTIL_H__ */