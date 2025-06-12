#include <stdint.h>
#include "uart.h"

#define NUM_ENTRIES    512
#define MAIR_NORMAL    0xFF       
#define MAIR_DEVICE    0x04       
#define MAIR_SHIFT     2

#define ACCESS_FLAG    (1ULL << 10)
#define UXN            (1ULL << 54)
#define VALID_DESC     (1ULL << 0)
#define BLOCK_DESC     (0ULL << 1)

__attribute__((aligned(4096))) static uint64_t l1_table[NUM_ENTRIES];

void mmu_init(void) {
    uart_puts("MMU init start\n");

    
    for (int i = 0; i < NUM_ENTRIES; i++) {
        l1_table[i] = 0;
    }

    
    asm volatile(
        "mov x1, %[mair0]\n"
        "orr x1, x1, %[mair1]\n"
        "msr mair_el1, x1\n"
        "isb\n"
        :
        : [mair0]"i"(MAIR_NORMAL),
          [mair1]"i"(MAIR_DEVICE << 8)
        : "x1", "memory"
    );
    uart_puts("MAIR set\n");

    
    l1_table[0] = (0ULL << 30)        
                | ACCESS_FLAG
                | UXN                 
                | BLOCK_DESC
                | VALID_DESC;

    
    l1_table[1] = (1ULL << 30)        
                | ACCESS_FLAG
                /* no UXN exec OK in EL0 */
                | BLOCK_DESC
                | VALID_DESC;
    uart_puts("L1 identity blocks set\n");

    
    asm volatile(
        "msr ttbr0_el1, %0\n"
        "isb\n"
        :: "r"(l1_table)
        : "memory"
    );
    uart_puts("TTBR0 set\n");

    
    asm volatile(
        "mov x1, #0x19\n"            
        "orr x1, x1, #(2 << 8)\n"    
        "orr x1, x1, #(2 << 10)\n"   
        "orr x1, x1, #(3 << 12)\n"   
        "msr tcr_el1, x1\n"
        "isb\n"
        ::: "x1", "memory"
    );
    uart_puts("TCR set\n");

    
    uart_puts("Enabling MMU...\n");
    asm volatile(
        "mrs x1, sctlr_el1\n"
        "orr x1, x1, #1\n"   
        "msr sctlr_el1, x1\n"
        "isb\n"
        ::: "x1", "memory"
    );
    uart_puts("MMU enabled\n");

     
    extern void exception_vector_table(void);
    asm volatile("msr vbar_el1, %0; isb" :: "r"(&exception_vector_table));

}


uint64_t mmu_get_l1_block(int i) {
    extern uint64_t l1_table[]; 
    return l1_table[i];
}