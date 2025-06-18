// mmu.c – two 1 GiB identity blocks, EL1 tables
#include <stdint.h>
#include "uart.h"

/* MAIR: Attr0 = normal WB/WA, Attr1 = device-nGnRnE */
#define MAIR_VALUE   ((0xFFULL << 0) | (0x04ULL << 8))

#define AF        (1ULL << 10)
#define UXN       (1ULL << 54)
#define AP_RW_EL0 (1ULL << 6)
#define BLOCK     (0ULL << 1)
#define VALID     (1ULL << 0)
#define ATTRIDX(n) ((uint64_t)(n) << 2)
#define L1_BLOCK(pa_gib, attr, extra) \
    (((uint64_t)(pa_gib) << 30) | AF | ATTRIDX(attr) | (extra) | BLOCK | VALID)

/* 4 KiB aligned L1 */
__attribute__((aligned(4096))) static uint64_t l1[512] = {0};

static inline void isb(void){ __asm__ volatile("isb"); }
static inline void write_mair(uint64_t v){ __asm__ volatile("msr mair_el1,%0"::"r"(v)); }
static inline void write_tcr (uint64_t v){ __asm__ volatile("msr tcr_el1,%0"::"r"(v)); }
static inline void write_ttbr0(void *p)  { __asm__ volatile("msr ttbr0_el1,%0"::"r"(p)); }

static inline void enable_mmu(void)
{
    __asm__ volatile(
        "mrs  x0, sctlr_el1\n"
        "orr  x0, x0, #1\n"   
        "bic  x0, x0, #(1<<28)\n" 
        "msr  sctlr_el1, x0");
    isb();
}

void mmu_init(void)
{
    uart_puts("MMU init start\n");

    /* 0-1 GiB for UART */
    l1[0] = L1_BLOCK(0, 1 /*Attr1*/, UXN);

    /* 1-2 GiB - normal memory */
    l1[1] = L1_BLOCK(1, 0, 0);

    uart_puts("L1 identity blocks set\n");

    write_mair(MAIR_VALUE);               uart_puts("MAIR set\n");

    write_tcr(0x3A19);                    uart_puts("TCR set\n");

    write_ttbr0(l1);                      uart_puts("TTBR0 set\n");

    uart_puts("Enabling MMU...\n");
    enable_mmu();                         uart_puts("MMU enabled\n");

    extern void exception_vector_table(void);
    __asm__ volatile("msr vbar_el1,%0"::"r"(&exception_vector_table));
    isb();
}

uint64_t mmu_get_l1_block(int i){ return l1[i]; }