// kernel.c

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
//void test_exec(void);

extern void enter_usermode(unsigned long pc, unsigned long sp)
        __attribute__((noreturn, naked));

        extern unsigned char _binary_user_shell_bin_start[];
extern unsigned char _binary_user_shell_bin_end[];

/* 0x8000 0000 user_stub.S */
/*extern void _user_start(void);
extern void process3(void);  */    

void print_mmu_blocks(void)
{
    uart_puts("MMU L1 Block 0: "); uart_hex(mmu_get_l1_block(0)); uart_puts("\n");
    uart_puts("MMU L1 Block 1: "); uart_hex(mmu_get_l1_block(1)); uart_puts("\n");
}

void kernel_main(void)
{

    size_t user_shell_size = _binary_user_shell_bin_end - _binary_user_shell_bin_start;
    unsigned char *dest = (unsigned char *)0x80000000;
    unsigned char *src = _binary_user_shell_bin_start;
    for (size_t i = 0; i < user_shell_size; ++i) {
        dest[i] = src[i];
    }
    uart_puts("User shell loaded to 0x80000000, size: ");
    uart_hex(user_shell_size);
    uart_puts(" bytes (hex)\n");

    uart_puts("First 16 bytes of user shell: ");
    for (size_t i = 0; i < 16; ++i) {
        uart_hex(dest[i]);
        uart_puts(" ");
    }
    uart_puts("\n");


    uart_init();
    //uart_puts("Booting…\n\n");

    /* subsystems that do not enable interrupts */
    mmu_init();
    process_init();
    vfs_init();
    init_process();

    // tests:
    /*test_vfs();
    test_messages();
    test_namespaces();
    test_ipc();
    test_exec();*/


    /* interactive shell while still in EL1 , comment out to test and work on EL0*/
    //uart_puts("\n=== kernel shell (EL1) — type \"exit\" to continue ===\n");
    //shell();                   
    //uart_puts("Shell exited; continuing boot…\n\n");

    /* enable timer/GIC and create EL0 task */
    timer_init();
    gic_init();

    // was for running shell in EL1
    /*process_t *user = process_create(_user_start);
    user->sp    = 0x80000000 + 0x10000;
    user->state = PROC_READY;*/

    // for EL0 process
    process_t *user = process_create((void*)0x80000000); // address, not _user_start!

    if (!user) {
        uart_puts("PANIC: Failed to create user process!\n");
        while (1) asm volatile ("wfe");
    }

    user->sp    = 0x80000000 + 0x10000;
    user->state = PROC_READY;


    uart_puts("user code @ 0x80000000: ");
    for (int i = 0; i < 16; ++i) {
        unsigned char b = ((volatile unsigned char*)0x80000000)[i];
        uart_hex(b);
        uart_puts(" ");
    }
    uart_puts("\n");

    /* drop to EL0  */
   // uart_puts("Dropping to EL0!\n");
   // uart_puts("user->ctx.pc: "); uart_hex(user->ctx.pc); uart_puts("\n");
   // uart_puts("user->sp:     "); uart_hex(user->sp);     uart_puts("\n");

    print_mmu_blocks();

    /*uart_puts("=== About to enter_usermode ===\n");
    uart_puts("Address of enter_usermode: ");
    uart_hex((unsigned long)&enter_usermode); uart_puts("\n");
    uart_puts("!!! PRE-JUMP !!!\n");*/

    uint64_t cur_el;
    asm volatile ("mrs %0, CurrentEL" : "=r"(cur_el));
    uart_puts("CurrentEL before eret: "); uart_hex(cur_el); uart_puts("\n");


    if ((cur_el & 4) == 0) {                
        uart_puts("PANIC: not in EL1!\n");
        for (;;)
            asm volatile("wfe");
    }

    extern char stack_top[];
    uart_puts("stack_top: "); uart_hex((unsigned long)stack_top); uart_puts("\n");

    extern char _bss_end[];
    uart_puts("_bss_end: "); uart_hex((unsigned long)_bss_end); uart_puts("\n");

    // user binary destination
    uart_puts("user_shell at: "); uart_hex((unsigned long)dest); uart_puts("\n");
    uart_puts("user stack top: "); uart_hex(user->sp); uart_puts("\n");

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

/* tests */

void test_vfs(void)
{
    if (ramfs_create_file("/tmp/test.txt", "Hello from RAMFS!") < 0) {
        uart_puts("Failed to create file\n");
        return;
    }
    int fd = vfs_open("/tmp/test.txt");
    if (fd < 0) {
        uart_puts("Failed to open file\n");
        return;
    }
    char buf[128];
    if (vfs_read(fd, buf, sizeof(buf)) < 0)
        uart_puts("Failed to read file\n");
}

void test_ramfs(void)
{
    if (ramfs_create_file("/tmp/test.txt", "Hello from RAMFS!") < 0) {
        uart_puts("Failed to create file\n");
        return;
    }
    int fd = vfs_open("/tmp/test.txt");
    if (fd < 0) {
        uart_puts("Failed to open file through VFS\n");
        return;
    }
    char buf[64] = {0};
    ssize_t n = vfs_read(fd, buf, sizeof(buf));
    if (n < 0)
        uart_puts("Failed to read file\n");
}

void test_messages(void)
{
    struct Message msg = { .type = MSG_OPEN, .path = "/hello.txt" };
    send_message(&msg);

    msg.type = MSG_READ;
    char buf[64];
    msg.data = buf;
    msg.size = sizeof(buf);
    send_message(&msg);
}

void test_namespaces(void)
{
    bind("/tmp", "/private", MREPL);

    struct Message msg = { .type = MSG_OPEN,
                           .path = "/tmp/test.txt",
                           .data = "X",
                           .size = 1 };
    send_message(&msg);

    msg.path = "/private/test.txt";
    send_message(&msg);
}

void test_ipc(void)
{
    struct process *p = get_current_process();
    if (!p) { uart_puts("No current process!\n"); return; }

    struct Message msg = { .type = 1, .data = "Hi", .size = 2 };
    queue_message(p, &msg);

    struct Message rcv = {0};
    receive_message(&rcv);
}

void test_processes(void)
{
    struct Message msg = { .type = MSG_FORK };
    int ret = send_message(&msg);

    if (ret == 0) {
        /* child */
        exit_process(42);
    } else {
        /* parent */
        msg.type = MSG_WAIT;
        send_message(&msg);
    }
}

/*void test_exec(void)
{
    struct Message msg = {
        .type  = MSG_EXEC,
        .path  = "/bin/test",
        .entry = (unsigned long)process3,
        .argv  = (char*[]){"test", NULL}
    };
    send_message(&msg);
}*/

/* old _user_entry stays but not used anymore */
void _user_entry(void)
{
    asm volatile("brk #0");
}
