-----------------------------

Aug 8 2015 update:

## Thread and it's pid:  
ANY -- (-1) used by server like FM, MM, PM  
idle -- 0  -- running when no other thread ready; init and run test;
send message to PM to make shell running; init file system;  
Timer -- 1  -- deal with time-related system call and interrupt  
TTY -- 2  -- deal with keyboard input and screen update  
getty -- 3-5  -- change input to upper case -- a test process for TTY  
IDE -- 6  -- deal with hard disk read/write  
RAMDISK -- 7  -- disk in RAM -- for test  
FM -- 8  -- deal with file related system call  
PM -- 9  -- deal with process related system call  
MM -- 10  -- deal with memory management  
~~user process invoker -- 11~~ -- change to idle to invoke it  
first user process -- 11  -- shell program -- bound to tty4, ie read from/write to it  
empty -- 12 -- do nothing but waiting for interrupt, used for avoiding deadlock @see src/kernel/process/util.c
- user process start from shell -- 13
-- now the file 1 is a `timer` , `exit()` and `args` test. input example: `1 asdf`  
-- file 2 is a test for `edf schedule method test`

-------------

## Drivers and devices:
- For a single process may take care different devices(e.g., TTY -- keyboard and screen)  
so pid is not able to identify a devices.  
- @deprecated ~~In my implementation, devices is uniquely identified by its **name**(or `pid & dev_id`),  
that is why `dev_read() & dev_write()` using `name` as first argument.~~  
- Now, devices can be uniquely identified by its **dev_id**, and avoid using old version of **name**.

-----------------

Aug 2 2015 update:

## State of a process
- running -- store in `current`
- ready -- store in `wake queue`
- sleeped -- `sleeped tree`, e.g. `receive()`, `sleep()`, all system call
- blocked on a semaphore -- linked by `pcb->link`, e.g. `P() & V()`


----------------------

## Virtual address
The kernel's virtual address space: [0xc000 0000, 0xffff ffff]  
The user process's virtual address space: [0x0, 0xbfff ffff]  


-----------------

The kernel default has **16M** memory
(which is the default setting of qemu)  

-----------------------

## File system
- boot and kernel is ahead of file system on the disk, ie file system  
doesn't take care of kernel which is simplified compared with reality(see following).  

```
size: sectors
0        1        A        A+1      A+256+1       A+1+256+1024
-------------------------------------------------------------------------------
         |        |        |        |            |           | user 
   MBR   | kernel | super  | iNode  |   block    |   iNode   | disk -- blocks
         |        | block  | bitmap |   bitmap   |           | space
-------------------------------------------------------------------------------
sizeof(iNode) == 128B
sizeof(block) == 1KB
```
- super block contains: inode map start, block map start, inode size, inode start, block size, block start, 
- i.e., now inode number is 2^12--4096, block number is 2^20
- operations to file system by `n_dev_read & n_dev_write` will add an offset of that section's start  
which is invisible for user.
- for most kernel process(pid [1-11] for the time being) are initialized before  
  file_system was loaded, in which `default_cwd` is setted, so for kernel thread,  
  `current working directory` are invalid for them. So have to reinitialize it again.  
- for a system call about change the content of a file(write related)  
  have to notice all the four areas(inode bitmap, block bitmap, inode, block).

### Implementation Problem

- @see file_table.h: FTE, file_syscall.c: contain_file(...);
When needing to open a file in the very deep directory, I need reading many directories inot memory.  
If I add every directory in the File Table(@see file_table.h), this would occupy much memory.  
But if I don't add it to file table, adding file to this directory making the file_size in the FTE out-dated.
-- may be I can add the last directory to file table, which maybe changed for adding/deleing directory.  

------------

## i386 ISA
1. push -- first minus 2/4 bytes, then store values, ie the esp always point to old value. If it is `push %esp`, it will save the old value before minus which means push original esp value.  
For example, in `do_irq.S`, `%esp` point to the start of TrapFrame and `push %esp` pushed the pointer of it.  


-------------------------

## user process interrupt stack
[user process stack state](pic/user_process_stack.jpg)  


--------------------

## process communication -- message

- receiver: `T f(Msg *m);` is the recommended form.  
	- return value is the most important information this function has( e.g. `PCB * create_process(Msg *);`)  
	- `m->ret` can be set in the function body, choose one from the enum in message.h, which is the state of this operation or detail error message  
    - if `m->ret` is `SUCC`, then `m->ret` can be set to `T`

