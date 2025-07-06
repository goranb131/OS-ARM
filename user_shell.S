.global _user_start
.section .text.start
_user_start:
    // print prompt char $ first
    mov x0, #'$'
    mov x8, #1           // syscall: putc
    svc #0

loop:
    mov x8, #2           // syscall: getc
    svc #0
    mov x1, x0           // save input char
    mov x0, x1           // set output char
    mov x8, #1           // syscall: putc
    svc #0
    b loop