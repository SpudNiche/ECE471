@ ECE 471 - HW 3
@ Author: Nicholas LaJoie 

@ Syscall defines
.equ SYSCALL_EXIT,     1


        .globl _start
_start:

        @================================
        @ Exit
        @================================
exit:

	mov r0, #42 		@ return this value (42) on exit by placing it in r0
	mov r7, #SYSCALL_EXIT   @ put exit system call, 1, in r7
	swi 0x0			@ make system call

