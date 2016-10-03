
FLAGS=-I../include
LDFLAGS=$(FLAGS) -nostdlib  -T../hal/$(PLATFORM)/linker.ld
CFLAGS=$(FLAGS) -g -O3 -ffreestanding -Wall -Wextra -Wno-unused-parameter
ASFLAGS=$(FLAGS)