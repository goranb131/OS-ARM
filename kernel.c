#include "uart.h"
#include "ramfs.h"
#include "timer.h"
#include "gic.h"
#include <stdint.h>
#include "mmu.h"
#include "process.h"
#include "vfs.h"
#include "kmalloc.h"
#include "abyssfs.h"
#include "message.h"
#include <stddef.h>
#include "shell.h"

void test_vfs(void);
void test_ramfs(void);
void test_abyssfs(void);
void test_messages(void);
void test_namespaces(void);
void test_ipc(void);
void test_processes(void);
void test_exec(void);

extern void enter_usermode(unsigned long pc, unsigned long sp)
        __attribute__((noreturn, naked));

/* 0x8000 0000 user_stub.S */
extern void _user_start(void);
extern void process3(void);      

void print_mmu_blocks(void)
{
    uart_puts("MMU L1 Block 0: "); uart_hex(mmu_get_l1_block(0)); uart_puts("\n");
    uart_puts("MMU L1 Block 1: "); uart_hex(mmu_get_l1_block(1)); uart_puts("\n");
}

void kernel_main(void)
{
    uart_init();
    uart_puts("ChthonOS v0.1.0\n");
    uart_puts("----------------\n");
    uart_puts("Booting...\n\n");

    mmu_init();
    process_init();
    vfs_init();
    init_process();

    test_vfs();
    test_messages();
    test_namespaces();
    test_ipc();
    test_exec();

    timer_init();
    gic_init();

    uart_puts("\nCreating user-mode process and scheduling...\n");

    process_t *user = process_create(_user_start);   /* PC = 0x8000 0000 */
    if (!user) {
        uart_puts("Failed to create user process!\n");
        for (;;) asm volatile("wfe");
    }
    user->sp    = 0x80000000 + 0x10000;   /* 64 KiB stack */
    user->state = PROC_READY;

    uart_puts("Dropping to EL0!\n");
    uart_puts("user->ctx.pc: "); uart_hex(user->ctx.pc); uart_puts("\n");
    uart_puts("user->sp:     "); uart_hex(user->sp);     uart_puts("\n");

    print_mmu_blocks();

    uart_puts("=== +About to enter_usermode ===\n");
    uart_puts("Address of enter_usermode: ");
    uart_hex((unsigned long)&enter_usermode); uart_puts("\n");
    uart_puts("!!! PRE-JUMP !!!\n");

    uint64_t cur_el;
    asm volatile ("mrs %0, CurrentEL" : "=r"(cur_el));
    uart_puts("CurrentEL before eret: "); uart_hex(cur_el); uart_puts("\n");

    if ((cur_el & 4) == 0) {                
        uart_puts("PANIC: not in EL1!\n");
        for (;;)
            asm volatile("wfe");
    }

    __asm__ __volatile__(
        "mov x0, %0\n"
        "mov x1, %1\n"
        "b   enter_usermode\n"
        :: "r"(user->ctx.pc), "r"(user->sp) : "x0", "x1");

    uart_puts("*** ERROR: returned from enter_usermode! ***\n");
    for (;;) asm volatile("wfe");
}

void handle_irq(void)
{
    uint64_t irq_status;
    asm volatile("mrs %0, cntp_ctl_el0" : "=r"(irq_status));
    if (irq_status & 4)
        timer_handler();
}

static struct vfs_super_block* test_mount(void)
{
    struct vfs_super_block* sb = kalloc(sizeof(*sb));
    if (!sb) return NULL;
    sb->s_magic   = 0x1234;
    sb->s_type    = "ramfs";
    sb->s_ops     = NULL;
    sb->s_fs_info = NULL;
    return sb;
}

void test_vfs(void)          { }
void test_ramfs(void)        { }
void test_messages(void)     {  }
void test_namespaces(void)   { }
void test_ipc(void)          {  }
void test_processes(void)    {  }

void test_exec(void)
{
    struct Message msg = {
        .type  = MSG_EXEC,
        .path  = "/bin/test",
        .entry = (unsigned long)process3,
        .argv  = (char*[]){"test", NULL}
    };
    send_message(&msg);
}

/* old _user_entry stays but not used anymore */
void _user_entry(void)
{
    asm volatile("brk #0");
}
