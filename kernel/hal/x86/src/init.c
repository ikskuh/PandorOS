#include <stdint.h>
#include "io.h"
#include "console.h"

struct idt
{
	uint16_t offset0; // 0-15	Offset 0-15	Gibt das Offset des ISR innerhalb des Segments an. Wenn der entsprechende Interrupt auftritt, wird eip auf diesen Wert gesetzt.
	uint16_t selector; // 16-31	Selector	Gibt den Selector des Codesegments an, in das beim Auftreten des Interrupts gewechselt werden soll. Im Allgemeinen ist dies das Kernel-Codesegment (Ring 0).
	uint8_t zero;
	uint8_t flags; // 32-34	000 / IST	Gibt im LM den Index in die IST an, ansonsten 0!
									// 35-39	Reserviert	Wird ignoriert
									// 40-42	Typ	Gibt die Art des Interrupts an
									// 43	D	Gibt an, ob es sich um ein 32bit- (1) oder um ein 16bit-Segment (0) handelt.
									// Im LM: Für 64-Bit LDT 0, ansonsten 1
									// 44	0	
									// 45-46	DPL	Gibt das Descriptor Privilege Level an, das man braucht um diesen Interrupt aufrufen zu dürfen.
									// 47	P	Gibt an, ob dieser Eintrag benutzt wird.
	uint16_t offset1; // 48-63	Offset 16-31	
};

static uint64_t gdt[] = {
	0x00, // NULL descriptor
	0x00CF9A000000FFFFULL,
	0x00CF92000000FFFFULL,
};

static struct idt idt[256];

static void init_gdt();
static void init_idt();

void x86_init()
{
	init_gdt();
	
	init_idt();
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

static void loop()
{
	printf("Hello Interrupt!\n");
	while(1);
}

static void timer_tick()
{
	printf("[tick]");
	outb(masterPIC, 0x20); // End of interrupt
	asm volatile(
		"add $8, %esp" "\n" 
		"iret");
}

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

static void init_idt()
{
	for(int i = 0; i < 256; i++)
	{
		idt[i] = idt_entry(&loop);
	}
	idt[0x20] = idt_entry(&timer_tick);
	
	lidt(idt, sizeof(idt));
	
	init_pic();
	
	asm volatile ("sti");
}