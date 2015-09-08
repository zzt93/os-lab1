# os-lab1
os-lab1

Try to learn os by lab

------------------------------

Aug 12 2015 update:

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




Implement some ADT:  
circular queue  
[BST](docs/ADT/BST.md)  
[map](docs/ADT/map.md)  
linked-list  
bit map  
[heap](docs/ADT/heap.md)

-----------------------------

Aug 8 2015 update:

## Thread and it's pid:  
ANY -- -1 used by server like FM, MM, PM  
idle -- 0  -- running when no other thread ready  
Timer -- 1  -- deal with time-related system call and interrupt  
TTY -- 2  -- deal with keyboard input and screen update  
getty -- 3-5  -- change input to upper case  
IDE -- 6  -- about hard disk  
RAMDISK -- 7  -- disk in RAM  
FM -- 8  -- deal with file related system call  
PM -- 9  -- deal with process related system call  
MM -- 10  -- deal with memory management  
user process invoker -- 11  
first user process -- 12  -- shell program -- bound to tty4, ie read from/write to it  


## Drivers and devices:
- For a single process may take care different devices(eg, TTY -- keyboard and screen)  
so pid is not able to identify a devices.  
- @deprecated In my implementation, devices is uniquely identified by its **name**(or `pid & dev_id`),  
that is why `dev_read() & dev_write()` using `name` as first argument.  
- Now, devices can be uniquely identified by its **dev_id**, and avoid using old version of **name**.

-----------------

Aug 2 2015 update:

## State of a process
- running -- store in `current`
- ready -- store in `wake queue`
- sleeped -- `sleeped tree`, eg `receive()`, `sleep()`, all system call
- blocked on a semaphore -- linked by `pcb->link`, eg `P() & V()`


----------------------

## Virtual address
The kernel's virtual address space: [0xc000 0000, 0xffff ffff]  
The user process's virtual address space: [0x0, 0xbfff ffff]  


-----------------

The kernel default has **16M** memory  

-----------------------

## File system
- boot and kernel is ahead of file system on the disk, ie file system  
doesn't take care of kernel which is simplified compared with reality(see following).  

```
size: sectors
0        1        A        A+1      A+256+1       A+1+256+1024
-----------------------------------------------------------------------
         |        |        |        |            |           | user 
   MBR   | kernel | super  | iNode  |   block    |   iNode   | disk
         |        | block  | bitmap |   bitmap   |           | space
-----------------------------------------------------------------------
sizeof(iNode) == 128B
```
- super block contains: inode map start, block map start, inode size, inode start, block size, block start, 
- ie, now inode number is 2**12, 4096, block number is 2**20
- operations to file system by `n_dev_read & n_dev_write` will add an offset of that section's start  
which is invisible for user.
- for most kernel process(pid [1-10]) are initialized before  
  file_system was loaded, in which `default_cwd` is  
  setted, so for kernel thread, `current working directory` are invalid for them  



## i386 ISA
1. push -- first minus 2/4 bytes, then store values, ie the esp always point to old value. If it is `push %esp`, it will save the old value before minus which means push original esp value.  
For example, in `do_irq.S`, `%esp` point to the start of TrapFrame and `push %esp` pushed the pointer of it.


-------------------------

## user process interrupt stack
[user process stack state](docs/pic/user_process_stack.jpg)


---------------------

## NOTICE in the future:
- the adt itself not must to be synchronized, the user choose whether to synchronize.  
- can add NOINTR to make sure user add synchronization if necessary  

- put the original global variable to parameter can better re-use some data structure.(ListHead)
- using macro when need different type(Tree<>)
- using special name convention when implement data structure, eg `_bitmap_array`


