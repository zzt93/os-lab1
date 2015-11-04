## Heap

### Usage:
   HEAP(T, capacity, f)
   T -- type  
   capacity -- the largest size of this heap  
   f -- function to compare elements, can be used to
   adjust max-heap or min-heap  

	- Invariant:  
		f(father, left_child) >= 0 && f(father, right_child) >= 0

	- This container is thread safe by `lock()&unlock()`
### Internal:
   - Using array to implement heap, ie priority queue  

   - array start from 0,  
   so if father index is `i`  
   left child is `2 * i + 1`  
   right child is `2 * i + 2`  
   
   - cmp_swap() -- this function compare father and  
   its two child(if have) and swap(if necessary).  
	So for percolate_up() -- in which a node only need compare with its father  
	, this function is inefficient.  
	

