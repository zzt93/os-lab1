#ifndef __TREE_H__
#define __TREE_H__

typedef struct TNode {
    struct TNode *left, *right;
} NodeLink;


#define ptr2container(ptr, type, member) \
    ((type*)( (char*)(ptr) - (int)(& ((type*)0)->member ) ))

#endif /* __TREE_H__ */
