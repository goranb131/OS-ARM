CC      = cc
AS      = cc
CFLAGS  = --target=aarch64-elf -march=armv8-a -ffreestanding -nostdlib -Iinclude
LDFLAGS = -fuse-ld=lld -T linker.ld

OBJS = boot.o enter_usermode.o kernel.o uart.o ramfs.o exceptions.o exceptions_c.o \
       timer.o gic.o mmu.o process.o context_switch.o vfs.o kmalloc.o \
       string.o abyssfs.o message.o namespace.o shell.o uart_debug.o user_shell_bin.o

# Default target
all: kernel.elf user_shell.bin

# Kernel build
boot.o: boot.S
	$(AS) $(CFLAGS) -c boot.S -o boot.o

enter_usermode.o: enter_usermode.S
	$(AS) $(CFLAGS) -c enter_usermode.S -o enter_usermode.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

uart.o: uart.c
	$(CC) $(CFLAGS) -c uart.c -o uart.o

ramfs.o: ramfs.c
	$(CC) $(CFLAGS) -c ramfs.c -o ramfs.o

exceptions.o: exceptions.S
	$(AS) $(CFLAGS) -c exceptions.S -o exceptions.o

exceptions_c.o: exceptions.c
	$(CC) $(CFLAGS) -c exceptions.c -o exceptions_c.o

timer.o: timer.c
	$(CC) $(CFLAGS) -c timer.c -o timer.o

gic.o: gic.c
	$(CC) $(CFLAGS) -c gic.c -o gic.o

mmu.o: mmu.c
	$(CC) $(CFLAGS) -c mmu.c -o mmu.o

process.o: process.c
	$(CC) $(CFLAGS) -c process.c -o process.o

context_switch.o: context_switch.S
	$(AS) $(CFLAGS) -c context_switch.S -o context_switch.o

process_test.o: process_test.c
	$(CC) $(CFLAGS) -c process_test.c -o process_test.o

vfs.o: vfs.c
	$(CC) $(CFLAGS) -c vfs.c -o vfs.o

kmalloc.o: kmalloc.c
	$(CC) $(CFLAGS) -c kmalloc.c -o kmalloc.o

string.o: string.c
	$(CC) $(CFLAGS) -c string.c -o string.o

abyssfs.o: abyssfs.c
	$(CC) $(CFLAGS) -c abyssfs.c -o abyssfs.o

message.o: message.c
	$(CC) $(CFLAGS) -c message.c -o message.o

namespace.o: namespace.c
	$(CC) $(CFLAGS) -c namespace.c -o namespace.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c -o shell.o

uart_debug.o: uart_debug.c
	$(CC) $(CFLAGS) -c uart_debug.c -o uart_debug.o

# User shell build (assembly version) 
user_shell.o: user_shell.S
	$(CC) $(CFLAGS) -c user_shell.S -o user_shell.o

user_shell.elf: user_shell.o
	$(CC) $(CFLAGS) -T user_linker.ld -nostdlib user_shell.o -o user_shell.elf

user_shell.bin: user_shell.elf
	llvm-objcopy -O binary user_shell.elf user_shell.bin

user_shell_bin.o: user_shell.bin
	llvm-objcopy --input-target binary --output-target elf64-littleaarch64 --binary-architecture aarch64 user_shell.bin user_shell_bin.o

# Kernel link 
kernel.elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o kernel.elf

clean:
	rm -f *.o kernel.elf user_shell.elf user_shell.bin