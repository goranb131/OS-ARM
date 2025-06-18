/* user_stub.S runs in EL0 and prints "U" once */

    .section .user, "ax"          
    .global  _user_start
_user_start:
    mov     x0, #'U'
    bl      uart_putc             

1:  wfi
    b       1b