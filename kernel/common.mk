
FLAGS=-I../include
LDFLAGS=$(FLAGS) -nostdlib  -T../hal/$(PLATFORM)/linker.ld
CFLAGS=$(FLAGS) -ffreestanding -Wall -Wextra
ASFLAGS=$(FLAGS)