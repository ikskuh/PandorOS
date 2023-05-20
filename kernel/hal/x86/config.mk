
# x86 configuration
SYSROOT=/opt/i686-elf
CC=clang -target i386-pc-none-elf -march=i686
#AS=$(SYSROOT)/bin/as -c -march i386 -msyntax=att
AS=clang -target i386-pc-none-elf -march=i686
LD=clang -target i386-pc-none-elf -march=i686