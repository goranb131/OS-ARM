.global enter_usermode
.type enter_usermode, %function
// x0 = entry PC (user->ctx.pc), x1 = user stack pointer (user->sp)
enter_usermode:
    mov     x2, #0                
    msr     spsr_el1, x2
    msr     sp_el0, x1            
    msr     elr_el1, x0           
    eret                          
    b .                            