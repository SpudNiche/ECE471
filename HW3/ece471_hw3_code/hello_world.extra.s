@ ECE 471 - HW 3
@ Author: Nicholas LaJoie

.arm

@ Syscall Definitions
.equ SYSCALL_EXIT,     1
.equ SYSCALL_WRITE,    4

@ Other Definitions
.equ STDOUT,	       1

        .globl _start
_start:

	@=========================================
	@ Something Cool: Descending Line Numbers
	@=========================================

	mov     r6,#12                  @ r6 is the line number variable, starting at 12
loop:
	mov r0,r6			@ Give r0 the line number argument
	cmp r0, #0			@ Printing down from line 12 to line 0
	blt exit

	bl	print_number		@ print r0 as a decimal number

	ldr	r1,=message		@ load message
	bl	print_string		@ print it

	sub r6,r6,#1			@ Increment Line Number
	b loop
        @================================
        @ Exit
        @================================
exit:
	mov	r0,#0			@ Return a zero
        mov	r7,#SYSCALL_EXIT	@
        swi	0x0			@ Run the system call

	@====================
	@ print_string
	@====================
	@ Null-terminated string to print pointed to by r1
	@ the value in r1 is destroyed by this routine

print_string:

	push    {r0,r2,r7,r10,lr}	@ Save r0,r2,r7,r10,lr on stack

	mov r4,r1			@ r4 contains base address of string
	mov r2,#0			@ r2 will contain the length of the string

count:
	ldrb r3,[r4]			@ load the current character of the string
	cmp r3,#0			@ check if '\0'
	beq done
	add r2,r2,#1			@ increment string counter
	add r4, r4, #1			@ increment address of r4
	b count				@ loop
done:
	mov	r0,#STDOUT		@ R0 Print to stdout
					@ R1 points to our string
					@ R2 is the length
	mov	r7,#SYSCALL_WRITE	@ Load syscall number
	swi	0x0			@ System call

	pop	{r0,r2,r7,r10,pc}       @ pop r0,r2,r3,pc from stack

.arm

        @#############################
	@ print_number
	@#############################
	@ r0 = value to print
	@ r0 is destroyed by this routine

print_number:
	push	{r10,LR}	@ Push r10 onto the stack
        ldr	r10,=buffer	@ Load 11-byte buffer address into r10
        add	r10,r10,#10	@ Add 10 to buffer address 

divide:
	bl	divide_by_10	@ Branch and link into 'divide_by_10' routine
	add	r8,r8,#0x30	@ Add ASCII 0x30 to remainder of 'divide_by_10'
	strb	r8,[r10],#-1	@ Store the ASCII value in the buffer, post
				@ update buffer to the right
	adds	r0,r7,#0	@ check if r7 is 0 
	bne	divide		@ continue division until number is complete 

write_out:
        add	r1,r10,#1	@ Add one to buffer address and store it in r1 

        bl	print_string	@ break and link to print_string routine, which will print r1

        pop	{r10,LR}	@ Pop r10 and lr

        mov	pc,lr		@ Link back to main program

	##############################
	# divide_by_10
	##############################
	# r0=numerator
        # r4,r5 trashed
	# r7 = quotient, r8 = remainder

divide_by_10:
	ldr	r4,=429496730		@ 1/10 * 2^32
	sub	r5,r0,r0,lsr #30
	umull	r8,r7,r4,r5		@ {r8,r7}=r4*r5

	mov	r4,#10			@ calculate remainder
	mul	r8,r7,r4
	sub	r8,r0,r8

	mov	pc,lr

.data
message:	.string ": ECE471 is cool\n" 

@ BSS
.lcomm buffer,11
