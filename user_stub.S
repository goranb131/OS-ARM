/* user_stub.S ─ runs in EL0, prints the current EL and "U" */

    .section .text.user
    .global  _user_start
_user_start:
    /* DEBUG */
    mrs   x0, CurrentEL       
    bl    uart_hex           
    mov   x0, #'\n'
    bl    uart_putc

    /* print U once, then sleep */
    mov     x0, #'U'
    bl      uart_putc

1:  wfi                        // stay in EL0 forever
    b       1b