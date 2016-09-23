.section .text

.extern x86_init
.extern os_init

.global _start
_start:
	mov $kernel_stack, %esp

	call x86_init
	
	call os_init

_stop:
	cli
	hlt
	jmp _stop

.section .bss
.space 8192
kernel_stack:
