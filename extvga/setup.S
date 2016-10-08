.section .text
 
.extern init

.global _start
_start:
    mov $kernel_stack, %esp
 
    call init
_stop:
    cli
    hlt
		jmp _stop

.section .bss
.space 8192
kernel_stack:
