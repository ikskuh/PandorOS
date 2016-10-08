
LD=/opt/i386-elf/bin/ld
AS=/opt/i386-elf/bin/as
CC=clang -target i386-pc-none-elf -march=i686

all: vga

vga: init.o setup.o
	$(LD) -Ttext=0x100000 -o $@ $^ -nostdlib

%.o: %.c
	$(CC) -c -o $@ $< -g -O3 -ffreestanding -Wall -Wextra -Wno-unused-parameter

%.o: %.S
	$(AS) -c -o $@ $<

run: vga
	qemu-system-i386 -kernel vga
	
.PHONY: vga