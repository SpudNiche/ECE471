# print_string_thumb.s

# Syscalls
.equ SYSCALL_EXIT,     1
.equ SYSCALL_WRITE,    4

# Other definitions
.equ STDOUT,           1

.thumb

        .globl _start
_start:
        ldr     r1,=hello
        bl      print_string            @ Print Hello World
        ldr     r1,=mystery
        bl      print_string            @
        ldr     r1,=goodbye
        bl      print_string            @ Print Goodbye


        #================================
        # Exit
        #================================
exit:
        mov     r0,#5
        mov     r7,#SYSCALL_EXIT        @ put exit syscall number (1) in eax
        swi     0x0                     @ and exit


        #====================
        # print string
        #====================
        # Null-terminated string to print pointed to by r1
        # r1 is trashed by this routine

print_string:

        push    {r0,r2,r3,r7,lr}        @ r10 changed to r3

        mov     r2,r1                   @

count_loop:
        ldrb    r3,[r2]                 @ r10 changed to r3
        add     r2,#1                   @ split out due to addressing mode

        cmp     r3,#0
        bne     count_loop
@ can't use cbnz, only branches forward



        sub     r2,r2,r1                @

        mov     r0,#STDOUT              @
        mov     r7,#SYSCALL_WRITE       @
        swi     0x0                     @

        pop     {r0,r2,r3,r7,pc}        @ r10 changed to r3


.data
hello:          .string "Hello World!\n"   @ includes null at end
mystery:        .byte 63,0x3f,63,10,0      @ mystery string
goodbye:        .string "Goodbye!\n"       @ includes null at end
