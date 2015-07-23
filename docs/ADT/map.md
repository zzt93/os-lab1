## map 

### Usage: MAP(K, V, name)
K -- key type  
V -- value type  
name -- map name  

### Internal
```

typedef struct {
    K k;
    V v;
} Entry;
```
   - Using the *binary search tree* to implement map,  
   the key of map must can be compared by  
   '<' '>' '='

   - Value might as well be a pointer, for the behaviour of  
   copying of v is just using `=`  

   - Duplicate value:  
   put duplicate key( which is decided by the function when initialize  
   it) will replace the old value  

