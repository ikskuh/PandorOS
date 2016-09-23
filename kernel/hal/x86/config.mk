
# x86 configuration
SYSROOT=/opt/i386-elf
CC=clang -target i386-pc-none-elf -march=i686
#AS=$(SYSROOT)/bin/as -march i386 -msyntax=att
AS=clang -target i386-pc-none-elf -march=i686
LD=$(SYSROOT)/bin/ld