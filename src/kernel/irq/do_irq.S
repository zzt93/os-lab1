	#include "x86/memory.h"
    #从i386手册中可以知道, 部分异常会在堆栈上压入错误码,
    #以提供更多的信息. 但并不是所有异常都会压入错误码, 为了把"陷阱帧"的结构统一起来,
    #我们为那些不会自动压入错误码的异常和外部中断都手动压入0
	#----|-----entry------|-errorcode-|-----id-----|---handler---|
	.globl vec0;    vec0:   pushl $0;  pushl    $0; jmp asm_do_irq
	.globl vec1;    vec1:   pushl $0;  pushl    $1; jmp asm_do_irq
	.globl vec2;    vec2:   pushl $0;  pushl    $2; jmp asm_do_irq
	.globl vec3;    vec3:   pushl $0;  pushl    $3; jmp asm_do_irq
	.globl vec4;    vec4:   pushl $0;  pushl    $4; jmp asm_do_irq
	.globl vec5;    vec5:   pushl $0;  pushl    $5; jmp asm_do_irq

    # invalid opcode: call -- dereference null
	.globl vec6;    vec6:   pushl $0;  pushl    $6; jmp asm_do_irq
	.globl vec7;    vec7:   pushl $0;  pushl    $7; jmp asm_do_irq
	.globl vec8;    vec8:              pushl    $8; jmp asm_do_irq
	.globl vec9;    vec9:   pushl $0;  pushl    $9; jmp asm_do_irq
	.globl vec10;   vec10:             pushl   $10; jmp asm_do_irq
	.globl vec11;   vec11:             pushl   $11; jmp asm_do_irq
	.globl vec12;   vec12:             pushl   $12; jmp asm_do_irq
	.globl vec13;   vec13:             pushl   $13; jmp asm_do_irq

    # general protection: cs has wrong content / page protection
	.globl vec14;   vec14:             pushl   $14; jmp asm_do_irq

	.globl vecsys; vecsys:  pushl $0;  pushl $0x80; jmp asm_do_irq

	.globl irq0;     irq0:  pushl $0;  pushl $1000; jmp asm_do_irq
	.globl irq1;     irq1:  pushl $0;  pushl $1001; jmp asm_do_irq
	.globl irq14;   irq14:  pushl $0;  pushl $1014; jmp asm_do_irq
	.globl irq_empty; irq_empty:pushl $0;pushl $-1; jmp asm_do_irq

	# the label "current" here is the content of variable current
	.extern current

	.globl asm_do_irq
	.extern irq_handle
    .extern lock
    
    # the following is for test printk in asm
    .extern printk
    .LC0:
	.string	"%x\n"

	asm_do_irq:
    # have to use cli, for using lock change some registers values
    cli
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal     #EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI

	movw $SELECTOR_KERNEL(SEG_KERNEL_DATA), %ax
	movw %ax, %ds
	movw %ax, %es

	pushl %esp #the parameter of irq_handle, tf
	call irq_handle

    # the following is push parameter to printk
    # movl current, %eax
    # movl (%eax), %ebx
    # pushl %ebx
    # movl	$.LC0, %edi
    # pushl %edi
    # call printk
    
	# YOU NEED TO SWITCH STACK TO current->tf
	# SO YOU NEED TWO ADD TWO LINES OF INTERRUPT CODE
	# HINT:
	#     1. USE movl INSTRUCTION
	#     2. USE (address) CAN REFERENCE MEMORY LOCATION OF address
	#     3. YOU MAY FLUSH ANY GENRAL PURPOSE REGISTER AS YOU WISH
	#     4. REGISTERS ARE REFERENCED BY "%", SUCH AS %esp

	################### your work ###################

	        movl current, %eax
	        movl (%eax), %esp

	#################################################
	    #addl $4, %esp # when you finish this task, this instruction should be removed
    # If you don't change the stack, the esp is the same, but you pushl the esp before call irq_handle, so add 4 to skip it.

	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	addl $8, %esp#remove #irq, and error code?

	iret
