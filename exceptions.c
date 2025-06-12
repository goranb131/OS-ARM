#ifndef _EXCEPTIONS_C
#define _EXCEPTIONS_C

#include "uart.h"
#include "exceptions.h"

typedef unsigned long uint64_t;

void handle_sync_exception(void) {
    uint64_t esr, far, elr;
    asm volatile("mrs %0, esr_el1" : "=r"(esr));
    asm volatile("mrs %0, far_el1" : "=r"(far));
    asm volatile("mrs %0, elr_el1" : "=r"(elr));
    uart_puts("\n*** SYNC EXCEPTION CAUGHT! ***\n");
    uart_puts("ESR_EL1: "); uart_hex(esr); uart_puts("\n");
    uart_puts("ELR_EL1: "); uart_hex(elr); uart_puts("\n");
    uart_puts("FAR_EL1: "); uart_hex(far); uart_puts("\n");
    
    while(1) asm volatile("wfe");
}

void handle_irq(void) {
    uart_puts("IRQ received!\n");
    while(1) { }  
}

#endif 