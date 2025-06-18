// mmu.c  – two 1 GiB identity blocks, block 0 = device, block 1 = normal
#include <stdint.h>
#include "uart.h"
/*
 *   AttrIdx0 = 0xFF  (normal)
 *   AttrIdx1 = 0x04  (device - nGnRnE)
*/
#define MAIR_VALUE  ((0xFFULL << 0) | (0x04ULL << 8))

/* page table attribute bits */
#define AF          (1ULL << 10)
#define UXN         (1ULL << 54)
#define BLOCK_DESC  (0ULL << 1)
#define VALID       (1ULL << 0)

#define ATTRIDX(n)  ((uint64_t)(n) << 2)     /* bits[4:2] in a block desc */
#define L1_BLOCK(pa_mb, attridx, extra) \
        (((uint64_t)(pa_mb) << 30) | AF | ATTRIDX(attridx) | (extra) | \
         BLOCK_DESC | VALID)

__attribute__((aligned(4096))) static uint64_t l1[512];

static inline void  isb(void)   { __asm__ volatile("isb"); }
static inline void  write_mair(uint64_t v)
{ __asm__ volatile("msr mair_el1,%0"::"r"(v):"memory"); }
static inline void  write_tcr (uint64_t v)
{ __asm__ volatile("msr tcr_el1,%0"::"r"(v):"memory"); }
static inline void  write_ttbr0(void *tbl)
{ __asm__ volatile("msr ttbr0_el1,%0"::"r"(tbl):"memory"); }
static inline void  enable_mmu(void)
{
    __asm__ volatile(
        "mrs x0, sctlr_el1\n"
        "orr x0, x0, #1\n"          /* set M   */
        "bic x0, x0, #(1<<28)\n"    /* clear SA0 (just in case) */
        "msr sctlr_el1, x0");
    isb();
}

void mmu_init(void)
{
    uart_puts("MMU init start\n");

    /* 0-1 GiB this is where UART is  */
    l1[0] = L1_BLOCK(0, 1 /*AttrIdx1*/, UXN);

    /* 1-2 GiB kernel text+data */
    l1[1] = L1_BLOCK(1, 0 /*AttrIdx0*/, 0);

    uart_puts("L1 identity blocks set\n");

    write_mair(MAIR_VALUE);      uart_puts("MAIR set\n");

    write_tcr(0x00000000808519ULL);  uart_puts("TCR set\n");

    write_ttbr0(l1);             uart_puts("TTBR0 set\n");

    uart_puts("Enabling MMU...\n");
    enable_mmu();                uart_puts("MMU enabled\n");

    extern void exception_vector_table(void);
    __asm__ volatile("msr vbar_el1,%0"::"r"(&exception_vector_table));
    isb();
}

uint64_t mmu_get_l1_block(int idx) { return l1[idx]; }