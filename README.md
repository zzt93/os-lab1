# os-lab1
os-lab1

Try to learn os by lab

------------------------------

Aug 2 2015 update:

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




Implement some ADT:  
circular queue  
[BST](docs/ADT/BST.md)  
[map](docs/ADT/map.md)  
linked-list  
bit map  
[heap](docs/ADT/heap.md)

-----------------------------

Jul 8 2015 update:

## Thread and it's pid:  
ANY -- -1 used by server like FM, MM, PM  
idle -- 0  -- running when no other thread ready  
Timer -- 1  -- deal with time-related system call and interrupt  
TTY -- 2  -- deal with keyboard input and screen  
getty -- 3-6  -- change input to upper case  
IDE -- 7  -- about hard disk  
RAMDISK -- 8  -- disk in RAM  
FM -- 9  -- deal with file related system call  
PM -- 10  -- deal with process related system call  
MM -- 11  -- deal with memory management  
user process invoker -- 12  
first user process -- 13  

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

## i386 ISA
1. push -- first minus 2/4 bytes, then store values, ie the esp always point to old value. If it is `push %esp`, it will save the old value before minus which means push original esp value.  
For example, in `do_irq.S`, `%esp` point to the start of TrapFrame and `push %esp` pushed the pointer of it.


-------------------------

## user process interrupt stack
[user process stack state](docs/pic/user_process_stack.jpg)


---------------------

## NOTICE in the future:
- the adt itself not must to be synchronized, the user choose whether to synchronize.  
- can add NOINTR to make sure user add synchronizatino


