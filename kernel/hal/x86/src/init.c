#include "io.h"
#include "interrupts.h"
#include "keyboard-drv.h"
#include "console.h"
#include <stdint.h>
#include <stddef.h>
#include "timer.h"

struct idt
{
	uint16_t offset0;
	uint16_t selector; 
	uint8_t zero;
	uint8_t flags;
	uint16_t offset1;
};

static uint64_t gdt[] = {
	0x00,                  // NULL Segment
	0x00CF9A000000FFFFULL, // Code Segment
	0x00CF92000000FFFFULL, // Data Segment
};

interrupt_f interrupts[256];

static struct idt idt[256];

static void init_gdt();
static void init_idt();

static struct cpu *timer_tick(struct cpu *cpu)
{
	os_tick();
	return cpu;
}

void x86_init()
{
	init_gdt();
	
	init_idt();
	
	interrupts[0x20] = &timer_tick;
	interrupts[0x21] = &keyboard_isr;
}

static void init_gdt()
{
	struct
	{
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) gdtp =
	{
		.limit = 3 * sizeof(uint64_t) - 1,
		.pointer = gdt,
	};
	asm volatile("lgdt %0" : : "m" (gdtp));
	
	asm volatile("ljmp $0x08, $reload_segments");
	asm volatile("reload_segments:");
	asm volatile("mov $0x10, %ax");
	asm volatile("mov %ax, %ds");
	asm volatile("mov %ax, %es");
	asm volatile("mov %ax, %fs");
	asm volatile("mov %ax, %gs");
	asm volatile("mov %ax, %ss");
}

static inline void lidt(void* base, uint16_t size)
{
	// This function works in 32 and 64bit mode
	struct {
			uint16_t length;
			void*    base;
	} __attribute__((packed)) IDTR = { size, base };
	asm ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

static struct idt idt_entry(void *ep)
{
	uint32_t offset = (uint32_t)ep;
	struct idt result;
	result.offset0  = (offset >>  0) & 0xFFFF;
	result.offset1  = (offset >> 16) & 0xFFFF;
	result.zero     = 0;
	result.selector = 0x08;
	result.flags    = 0x8E;
	return result;
}
uint8_t masterPIC = 0x20;
uint8_t slavePIC = 0xA0;

static void init_pic()
{
	// Setup master
	outb(masterPIC + 0x00, 0x11);
	outb(masterPIC + 0x01, 0x20);
	outb(masterPIC + 0x01, 0x04);
	outb(masterPIC + 0x01, 0x01);
	
	// Setup slave
	outb(slavePIC + 0x00, 0x11);
	outb(slavePIC + 0x01, 0x28);
	outb(slavePIC + 0x01, 0x02);
	outb(slavePIC + 0x01, 0x01);
	
	// Unmask all
	outb(masterPIC + 0x01, 0x00);
	outb(slavePIC + 0x01, 0x00);
}

#define ISR(n) void isr_##n();
#define ISR_ERR(n) ISR(n)
#include "../lists/interrupts"
#undef ISR_ERR
#undef ISR

static void init_idt()
{
#define ISR(n) idt[n] = idt_entry(isr_##n);
#define ISR_ERR(n) ISR(n)
#include "../lists/interrupts"
#undef ISR_ERR
#undef ISR
	
	lidt(idt, sizeof(idt));
	
	init_pic();
	
	asm volatile ("sti");
}

struct cpu * interrupt_dispatch(struct cpu * cpu)
{
	// Call interrupt handler
	if(interrupts[cpu->interrupt] != NULL) {
		cpu = interrupts[cpu->interrupt](cpu);
	} else {
		printf("\n[Unhandled interrupt: %d]\n", cpu->interrupt);
	}
	
	// ACK PICs if necessery
	if(cpu->interrupt >= 0x20 && cpu->interrupt <= 0x2F) {
		if(cpu->interrupt >= 0x28) {
			outb(slavePIC, 0x20); // ACK Slave PIC
		}
		outb(masterPIC, 0x20); // ACK Master PIC
	}
	
	return cpu;
}


