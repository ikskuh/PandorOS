
FLAGS=-I../include
LDFLAGS=$(FLAGS) -nostdlib  -T../hal/$(PLATFORM)/linker.ld
CFLAGS=$(FLAGS) -O3 -ffreestanding -Wall -Wextra
ASFLAGS=$(FLAGS)