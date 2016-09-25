
.macro isr_stub nr
.global isr_\nr
isr_\nr:
    pushl $0
    pushl $\nr
    jmp isr_handler
.endm

.macro isr_stub_with_err nr
.global isr_\nr
isr_\nr:
    pushl $\nr
    jmp isr_handler
.endm

#define ISR(num) isr_stub num
#define ISR_ERR(num) isr_stub_with_err num
#include "../lists/interrupts"
#undef ISR
#undef ISR_ERR

.extern interrupt_dispatch

isr_handler:
	// Store CPU State
	push %ebp
	push %edi
	push %esi
	push %edx
	push %ecx
	push %ebx
	push %eax

	// Calls interrupt handler with CPU state as argument
	push %esp
	call interrupt_dispatch
	// add $4, %esp
	mov %eax, %esp


	// Restore CPU State
	pop %eax
	pop %ebx
	pop %ecx
	pop %edx
	pop %esi
	pop %edi
	pop %ebp

	// Remove error code and interrupt number
	add $8, %esp

	iret