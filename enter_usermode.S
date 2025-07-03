// enter_usermode.S – minimal, relocation-free
    .text
    .global enter_usermode
    .type   enter_usermode,%function

/* void enter_usermode(uint64_t pc, uint64_t sp) */
enter_usermode:
    msr     sp_el0,  x1        // user stack
    msr     elr_el1, x0        // user PC
    
    // can unmask interrupts because kernel is already set up to safely handle interrupts, preempt, and resume user code
    mov     x2,      #0        // SPSR = EL0t, IRQs enabled
    //msr     spsr_el1, x2

    //for masking interrupts
    //mov     x2, #(1 << 7) // SPSR = EL0t, IRQs disabled (I=1)
    msr     spsr_el1, x2
    isb
    eret                       // EL0, never returns