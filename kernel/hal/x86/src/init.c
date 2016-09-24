#include <stdint.h>

uint64_t gdt[] = {
	0x00, // NULL descriptor
	0x00CF9A000000FFFFULL,
	0x00CF92000000FFFFULL,
};

uint64_t idt[256];

static void init_gdt();
static void init_idt();

void x86_init()
{
	init_gdt();
	
	// init_idt();
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

static void init_idt()
{
	lidt(idt, sizeof(idt));
}