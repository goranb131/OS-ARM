#include <stdint.h>
#include "uart.h"

#define EC_SVC64           0x15
#define SYS_UART_PUTC      1
#define SYS_UART_GETC      2

uint64_t handle_sync_exception(uint64_t user_x0, uint64_t user_x8)
{
    uart_puts("[SVC]\n");
    uint64_t esr_el1;
    asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
    uint32_t ec  = (esr_el1 >> 26) & 0x3F;

    if (ec == EC_SVC64) {
        switch (user_x8) {
        case SYS_UART_PUTC:
            uart_putc((char)user_x0);
            return 0;
        case SYS_UART_GETC:
            return uart_getc();
        }
    }
    while (1) asm volatile("wfe");
    return 0;
}