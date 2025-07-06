/* user_stub.S – EL0 demo */
    .section .user, "ax"          // first time: A|X
    .global  _user_start
_user_start:
    adr     x0, hello_msg
    bl      uart_puts
    mov     x0, #'\n'
    bl      uart_putc
1:  wfi
    b       1b

    /* reopen with the same flags */
    .section .user, "ax"          
hello_msg:
    .asciz "Hello from EL0"