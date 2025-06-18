// exceptions.S – EL1 vector table + tiny diagnostic print
    .section .text
    .align   11                       // 2 KiB alignment (QEMU requirement)
    .global  exception_vector_table
exception_vector_table:

/*  EL1 (SP0)  */
    .align 7 ;  b sync_handler        
    .align 7 ;  b .                   
    .align 7 ;  b .                   
    .align 7 ;  b .                   

/* EL1 (SPx) */
    .align 7 ;  b sync_handler
    .align 7 ;  b curr_el_irq
    .align 7 ;  b .
    .align 7 ;  b .

/* Lower EL (EL0)  */
    .align 7 ;  b sync_handler
    .align 7 ;  b curr_el_irq
    .align 7 ;  b .
    .align 7 ;  b .

/* Lower EL AArch32 unused */
    .align 7 ;  b .
    .align 7 ;  b .
    .align 7 ;  b .
    .align 7 ;  b .


sync_handler:
    sub     sp, sp, #32
    stp     x0, x1, [sp]         
    stp     x2, x3, [sp,#16]

    mrs     x0, esr_el1          
    mrs     x1, far_el1         
    bl      uart_dump_esr_far   

    mrs     x0, esr_el1
    mrs     x1, elr_el1
    bl      handle_sync_exception

    ldp     x2, x3, [sp,#16]
    ldp     x0, x1, [sp]
    add     sp, sp, #32
    eret

curr_el_irq:
    sub     sp, sp, #16
    stp     x30, xzr, [sp]       
    bl      handle_irq
    ldp     x30, xzr, [sp]
    add     sp, sp, #16
    eret

    .global uart_dump_esr_far
    .global uart_dump_esr_far
uart_dump_esr_far:
    // x0 = ESR_EL1, x1 = FAR_EL1
    adr     x2, dump_msg
1:  ldrb    w3, [x2], #1
    cbz     w3, 2f
    bl      uart_putc
    b       1b
2:  mov     x3, x0
    bl      uart_hex
    mov     x0, #' '
    bl      uart_putc
    mov     x3, x1
    bl      uart_hex
    mov     x0, #'\n'
    bl      uart_putc
    ret

dump_msg: .asciz "SYNC: "


    .global handle_sync_exception
handle_sync_exception:
    ret                            

