#ifndef __LIST_H__
#define __LIST_H__

#include "assert.h"

struct ListHead {
    struct ListHead *prev, *next;
};
typedef struct ListHead ListHead;

/**
  return the pointer to the container struct, ie type*
  eg PCB *pcb = list_entry(ptr, PCB, link)
  @NOTICE: never surround type and member with () or
  will be `macro expected expression before ‘)’ token`
 */
#define list_entry(ptr, type, member) \
    ((type*)( (char*)(ptr) - (int)(& ((type*)0)->member ) ))

static inline void
list_add(ListHead *prev, ListHead *next, ListHead *data) {
    assert(data != NULL);
    data->prev = prev;
    data->next = next;
    if (prev != NULL) prev->next = data;
    if (next != NULL) next->prev = data;
}

static inline void
list_add_before(ListHead *list, ListHead *data) {
    assert(list != NULL);
    list_add(list->prev, list, data);
}

static inline void
list_add_after(ListHead *list, ListHead *data) {
    assert(list != NULL);
    list_add(list, list->next, data);
}

static inline void
list_del(ListHead *data) {
    assert(data != NULL);
    ListHead *prev = data->prev;
    ListHead *next = data->next;
    if (prev != NULL) prev->next = next;
    if (next != NULL) next->prev = prev;
}

static inline void
list_init(ListHead *list) {
    assert(list != NULL);
    list->prev = list->next = list;
}

static inline bool
list_empty(ListHead *list) {
    assert(list != NULL);
    return list == list->next;
}


#define list_foreach(ptr, head) \
    for ((ptr) = (head)->next; (ptr) != (head); (ptr) = (ptr)->next)

static inline int list_size(ListHead *list) {
    assert(list != NULL);
    int count = 0;
    ListHead *p = NULL;
    //printk("%x ", list);
    list_foreach(p, list) {
        //printk("%x ", p);
        assert(p != NULL && p != p->next);
        count++;
    }
    return count;
}

/**
   head: the *value* of a ListHead pointer
   T: the type name
   member: the member in the T
 */
#define list_free(head, T, member) {            \
        ListHead *p = (head)->next;             \
        T *t = NULL;                            \
        while ((p) != (head)) {                 \
            t = list_entry((p), T, member);     \
            (p) = (p)->next;                    \
            kfree(t);                           \
        }                                       \
    }                                           \

#define list_copy(head, T, member, dest) {          \
        ListHead *p = NULL;                         \
        T *t_src = NULL;                            \
        T *t_dest = NULL;                           \
        list_foreach((p), head) {                   \
            t_src = list_entry((p), T, member);     \
            t_dest = kmalloc(sizeof(T));            \
            memcpy(t_dest, t_src, sizeof(T));       \
            list_add_after(dest, &t_dest->member);  \
        }                                           \
    }

#endif

