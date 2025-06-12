# Makefile LLD/Clang
CC      = clang
AS      = clang
CFLAGS  = --target=aarch64-elf -march=armv8-a -ffreestanding -nostdlib -Iinclude
LDFLAGS = -fuse-ld=lld -T linker.ld

OBJS = boot.o enter_usermode.o kernel.o uart.o ramfs.o exceptions.o timer.o gic.o mmu.o process.o context_switch.o process_test.o vfs.o kmalloc.o string.o abyssfs.o message.o namespace.o shell.o

all: kernel.elf

boot.o: boot.S
	$(AS) $(CFLAGS) -c boot.S -o boot.o

enter_usermode.o: enter_usermode.S
	$(AS) $(CFLAGS) -c $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

uart.o: uart.c
	$(CC) $(CFLAGS) -c uart.c -o uart.o

ramfs.o: ramfs.c
	$(CC) $(CFLAGS) -c ramfs.c -o ramfs.o

exceptions.o: exceptions.S
	$(AS) $(CFLAGS) -c exceptions.S -o exceptions.o

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

kernel.elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o kernel.elf

clean:
	rm -f *.o kernel.elf