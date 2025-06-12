// exceptions.S

    .section .text
    .align 11                // 2 KiB alignment
    .global exception_vector_table
exception_vector_table:

    // EL1, SP0
    .align 7
    b sync_handler
    .align 7
    b .                      // IRQ
    .align 7
    b .                      // FIQ
    .align 7
    b .                      // SError

    // EL1, SPx
    .align 7
    b sync_handler
    .align 7
    b curr_el_irq
    .align 7
    b .
    .align 7
    b .

    // (EL0) AArch64
    .align 7
    b sync_handler           
    .align 7
    b curr_el_irq            
    .align 7
    b .                      // FIQ
    .align 7
    b .                      // SError

    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .

sync_handler:
    sub sp, sp, #16*16
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x30, xzr, [sp, #32]
    mrs x0, esr_el1
    mrs x1, elr_el1
    bl  handle_sync_exception
    ldp x0, x1, [sp, #0]
    ldp x2, x3, [sp, #16]
    ldp x30, xzr, [sp, #32]
    add sp, sp, #16*16
    eret

curr_el_irq:
    sub sp, sp, #16*16
    bl  handle_irq
    add sp, sp, #16*16
    eret

.global handle_sync_exception
handle_sync_exception:
    ret

.section .rodata
sync_msg:
    .asciz "Sync exception! ESR_EL1: "
elr_msg:
    .asciz "\nELR_EL1: "
newline:
    .asciz "\n"
.align 4