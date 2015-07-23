
##BST -- binary search tree

### Usage: BI_TREE(T, cmp, name);

  T -- the type parameter  
  cmp -- the comparing funciton, bool f(T, T);  
  name -- the name of tree
  
### Internal
```

typedef struct {
    T t;
    NodeLink link;
} TNode_##name;
```
  - the order: forbidden to add same element  
  left < mid < right

  - no duplicate:  
  if there are more than one same elements -- defined by your  
  comparing function -- added, only first is added  

  - implementation detail:  
    - 
    - use a TNode_name* as the head, and only use the **left link**  


