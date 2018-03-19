[![Build Status](https://travis-ci.org/zzt93/os-lab1.svg?branch=master)](https://travis-ci.org/zzt93/os-lab1)

# os-lab1
os-lab1

Try to learn os by lab

------------------------------

## Aim:
![Big picture](docs/pic/aim.png)


## Running requirement
- make  
- qemu( I am using `qemu-system-i386` in makefile)  
- gcc  

----------------------

Sep 26 2015 update:

## The functionalities that I have already implemented:

### Mis
- printk: printf in kernel  
- kmalloc: malloc in kernel, using next-fit algorithm. If write memory out of the requested range, may crash the `kmalloc`, so this can be only used by kernel. User space `malloc` should be guarded by virtual page protection.
- the key press interrupt handler: handler have to be short and fast; have to disable interrupt; 
- a simple maze game running without os  
- the context switch between threads  
- the creation of kernel thread: the core is to init a faked `TrapFrame`, which is allocated on `kstack`.
- the schedule of thread: now using Round-Robin algorithm; add EDF algo.
- the sleep and wake_up of thread: add process to separate `Binary Tree` when schedule.
- the semaphore(`P`&`V`) and lock(`cli`&`sti`) to solve critical section problem.
    - nested lock: counter
    - sleep during lock: per thread counter
    - locking in interrupts: increase counter when enter interrupt handle; decrease when leave. `lock & unlock` have to in pair!
- the send and receive message between threads: send is asynchronous and will always succeed; receive is synchronous and may be blocked.

- add timer (implemented by priority queue) -- clock diver, ide -- hard disk driver, tty -- terminal driver  
- very very simple file system in ram: name is number, size is fixed; upgrade to ext2-like version.
- create user process: user's stack is stored in `TrapFrame`. 
- using memory page dynamic allocation: using bit-map algorithm  
- add page protection: use page table & page directory with flag specified.
- put user in ring3: page protection & register `ss`
- wait(int second) -- system call  
- random number  
- non-blocked timer: used to make a process run specific seconds, i.e. counting its running time  
- LCM and GCD  

### process
- fork a process: share fd, read-only page etc.
- exec a command: free process resource + create process
- exit a user process: free process resource
- waitpid: waiting util a process exit.
- a simple shell: a user process called fork and exce.

### file system
- upgrade file system: change file system name to string; size is variable  
- create a file(file name max is 28 bytes @see f_dir.h)  
- make a directory(directory depth max is 16 @see f_dir.h)  
- delete a file  
- delete a directory  
- change current working directory  
- list files under a folder  
- open a file  
- close a file  
- read a file  
- write a file  
- lseek a file: i.e. change the cursor for reading/writing  

### shell utility:
- cd
- pwd
- ls
- echo
- echo arguments and wait 5 seconds


### Implement some ADT:  
- circular queue  
- [BST](docs/ADT/BST.md)  
- [map](docs/ADT/map.md)  
- linked-list  
- bit map  
- dynamic allocated bit map (using kmalloc)  
- [heap](docs/ADT/heap.md)

---------------------

[some important implementation details](docs/internal.md)

-----------------

## NOTICE in the future:
- the adt itself may not to be synchronized, the user choose whether to synchronize it, like `Vector and ArrayList`?  
- can add NOINTR to make sure user add synchronization if necessary  
- the adt seems should not using semaphore and P&V for concurrent use, for it may used in irq which may cause deadlock  

- put the original global variable to parameter can better re-use some data structure.(ListHead)
- using macro when need different type(Tree<>)
- using special name convention when implement data structure, e.g. `_bitmap_array` to avoid name collision  
- using `name##_adt` can better reuse adt  

- design an error return system between 'client' and 'server'(in os is 'application programmer' and 'os kernel programmer')
	- `enum`: not a simple `FAIL` and `SUCC` but `no such directory or file`, `Segmentation fault`.
	- `struct`: a int indicate state, and a string indicate error message.

- if fail to allocate some resources when allocating a serial of resources,  
	remember to free already allocated resources.  

- avoid using `void *` for it may hide some bugs because the implicit conversion of different type of pointers.  

- if not sure what type should use, may use macro called like `xxx_t` to make it easy to change  


## Debug
- minimize the code where bug might in by comment other parts
- rational analysis about who can change it
- gdb: watch and breakpoint
- print more if bug is related to interrupt and not so easy to repeat
- exceptions like 14, 13: 1. printed stack info 2. error code with i386 manual

### Bug
- no newline for shell error message
- a single enter no prompt

### TODO
- make `lock()` support in interrupt handling
- load program when it needs -- 缺页
- command: mkdir, rm, touch, >, vi
- network: tcp/ip, socket -- ping, traceroute
  - loopback interface
  - driver code
  - kernel code: protocol impl
  - system call
  - command: ping, traceroute
  
  
---

## Run
```bash
$ make run

# press F1~F4 to change tty, the last tty has the shell
```