    .section .text
    .align   11
    .global  exception_vector_table

exception_vector_table:
    // EL1t 
    .org 0x000
    b   .

    .org 0x080
    b   .

    .org 0x100
    b   .

    .org 0x180
    b   .

    // EL1h
    .org 0x200
    b   .

    .org 0x280
    b   .

    .org 0x300
    b   .

    .org 0x380
    b   .

    // EL0 64-bit Sync (SVC)
    .org 0x400
sync_el0_64:
    // save user SP (for later restoration)
    mov x21, sp

    // kernel stack, not user stack
    ldr x22, =stack_top      // kernel stack top 
    mov sp, x22

    // save user context (x0-x30)
    sub sp, sp, #(32*8)
    stp x0, x1,   [sp, #( 0*8)]
    stp x2, x3,   [sp, #( 2*8)]
    stp x4, x5,   [sp, #( 4*8)]
    stp x6, x7,   [sp, #( 6*8)]
    stp x8, x9,   [sp, #( 8*8)]
    stp x10, x11, [sp, #(10*8)]
    stp x12, x13, [sp, #(12*8)]
    stp x14, x15, [sp, #(14*8)]
    stp x16, x17, [sp, #(16*8)]
    stp x18, x19, [sp, #(18*8)]
    stp x20, x21, [sp, #(20*8)] // x21 = user sp (saved)
    stp x22, x23, [sp, #(22*8)]
    stp x24, x25, [sp, #(24*8)]
    stp x26, x27, [sp, #(26*8)]
    stp x28, x29, [sp, #(28*8)]
    str x30,      [sp, #(30*8)] // x30 (LR)
    // save elr_el1 and spsr_el1 (return addr, status)
    mrs x0, elr_el1
    str x0, [sp, #(31*8)]
    mrs x0, spsr_el1
    str x0, [sp, #(32*8)]

    // args for C: x0=user x0, x1=user x8
    ldr x0, [sp, #(0*8)]
    ldr x1, [sp, #(8*8)]

    bl handle_sync_exception

    // store result back to user x0 slot
    str x0, [sp, #(0*8)]

    // restore context (x0-x30)
    ldp x0, x1,   [sp, #( 0*8)]
    ldp x2, x3,   [sp, #( 2*8)]
    ldp x4, x5,   [sp, #( 4*8)]
    ldp x6, x7,   [sp, #( 6*8)]
    ldp x8, x9,   [sp, #( 8*8)]
    ldp x10, x11, [sp, #(10*8)]
    ldp x12, x13, [sp, #(12*8)]
    ldp x14, x15, [sp, #(14*8)]
    ldp x16, x17, [sp, #(16*8)]
    ldp x18, x19, [sp, #(18*8)]
    ldp x20, x21, [sp, #(20*8)] // x21 = user sp
    ldp x22, x23, [sp, #(22*8)]
    ldp x24, x25, [sp, #(24*8)]
    ldp x26, x27, [sp, #(26*8)]
    ldp x28, x29, [sp, #(28*8)]
    ldr x30,      [sp, #(30*8)]
    ldr x0, [sp, #(31*8)]
    msr elr_el1, x0
    ldr x0, [sp, #(32*8)]
    msr spsr_el1, x0

    // restore user sp
    mov sp, x21

    eret

    .global handle_sync_exception   // C function