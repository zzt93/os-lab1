# os-lab1
os-lab1

Try to learn os by lab

------------------------------

## Aim:
[Big picture](docs/pic/aim.png)


## Running requirement
- make
- qemu( I am using `qemu-system-i386` in makefile)
- gcc  
`
----------------------

Sep 26 2015 update:

## The functionalities that I have already implemented:

printk -- printf in kernel  
kmalloc -- malloc in kernel -- using next-fit algorithm  
the key press interrupt handler  
a simple maze game running without os  
the context switch between threads  
the creation of thread  
the schedule of thread -- now using Round-Robin algorithm  
the sleep and wake_up of thread  
the semaphore(P&V) and lock(cli&sti) to slove critical section  
the send and receive between threads  -- send it asynchronized and will always succeed; receive is synchronized and may be blocked   
add timer-- clock diver, ide-- hard disk driver, tty -- terminal driver  
very very simple file system  
user process  
using page allocation  -- using bit-map algorithm  
add page protection  
put user in ring3  
wait(int second) -- system call  
random number  
fork a process  
exec a command  
exit a user process  
waitpid  
create a file  
make a directory  
delete a file  
delete a directory  
change current working directory  
list current working directory  




###Implement some ADT:  
circular queue  
[BST](docs/ADT/BST.md)  
[map](docs/ADT/map.md)  
linked-list  
bit map  
dynamic allocated bit map( using kmalloc)  
[heap](docs/ADT/heap.md)

---------------------

[some important implementation details](docs/internal.md)

-----------------

## NOTICE in the future:
- the adt itself not must to be synchronized, the user choose whether to synchronize.  
- can add NOINTR to make sure user add synchronization if necessary  

- put the original global variable to parameter can better re-use some data structure.(ListHead)
- using macro when need different type(Tree<>)
- using special name convention when implement data structure, e.g. `_bitmap_array`

- design an error return system between 'client' and 'server'(in os is 'application programmer' and 'os kernel programmer')
	- `enum`: not a simple `FAIL` and `SUCC` but `no such directory or file`, `Segmentation fault`.
	- `struct`: a int indicate state, and a string indicate error message.

- if fail to allocate some resources when allocating a serial of resources,  
	remember to free already allocated resources.

- avoid using `void *` for it may hide some bugs because the implicit conversion of different type of pointers.
