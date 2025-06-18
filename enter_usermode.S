/* enter_usermode.S  –  called from C with
 *     x0 = entry-PC in user space
 *     x1 = top of user stack
 */

    .text
    .global enter_usermode
    .type   enter_usermode, %function

enter_usermode:
    msr     sp_el0,  x1          // user stack
    msr     elr_el1, x0          // user PC
    mov     x2,  #0              // interrupts enabled
    msr     spsr_el1, x2

    isb                         

    /* debug  */
    mrs     x0, spsr_el1
    mrs     x1, elr_el1
    mrs     x2, sp_el0
    bl      uart_dump_regs

    /* eret */
    eret                      

still_el1:
    bl      uart_puts
    .asciz  "*** STILL IN EL1 AFTER ERET ***\n"
    .p2align 2
    b       .

    .size   enter_usermode, . - enter_usermode