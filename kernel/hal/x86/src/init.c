#include "io.h"
#include "interrupts.h"
#include "keyboard-drv.h"
#include "console.h"
#include "multiboot.h"
#include "timer.h"
#include "pmm.h"
#include "printf.h"
#include "options.h"
#include <stdint.h>
#include <stddef.h>


// Prüft, ob man bereits schreiben kann
uint8_t is_transmit_empty() {
	return inb(0x3F8+5) & 0x20;
}

// Byte senden
void serial_putc(char ch) {
	while (is_transmit_empty()==0);
	outb(0x3F8, ch);
}

void hal_debug(char const *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	gprintf(serial_putc, fmt, list);
	va_end(list);
}


struct multiboot_header __attribute__((section ("multiboot"))) multibootHeader __attribute__ ((aligned (4))) = {
	MULTIBOOT_HEADER_MAGIC, 
	0x00,
	-(MULTIBOOT_HEADER_MAGIC + 0x00),
	
	/* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
	0, // header_addr;
	0, // load_addr;
	0, // load_end_addr;
	0, // bss_end_addr;
	0, // entry_addr;

	/* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
	0, // mode_type;
	0, // width;
	0, // height;
	0, // depth;
};

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
static void init_pmm(struct multiboot_info const * info);

static struct cpu *timer_tick(struct cpu *cpu)
{
	os_tick();
	return cpu;
}

optiongroup_t halOptions = {
	"x86 Options", NULL, NULL
};

extern option_t halOptConsoleForeground;
extern option_t halOptConsoleBackground;
extern option_t halOptConsoleHighlight;
extern option_t halOptConsoleBoldtext;


extern bool kbddrvLogKeys;

option_t halOptKeyboardLog = {
	OPT_BOOL, "Log Keyboard", &kbddrvLogKeys, NULL, NULL
};

void x86_init(uint32_t bootmagic, struct multiboot_info const * info)
{
	// Checks for a correct multiboot magic
	if(bootmagic != MULTIBOOT_BOOTLOADER_MAGIC) {
		uint16_t *video = (uint16_t*)0xB8000;
		char *msg = "Invalid boot magic!";
		while(*msg) {
			*video++ = (*msg++) | 0xFC00;
		}
		while(true);
	}
	
	init_gdt();
	
	init_pmm(info);
	
	init_idt();
	
	interrupts[0x20] = &timer_tick;
	interrupts[0x21] = &keyboard_isr;
	
	optiongroup_register(&halOptions);
	
	option_add(&halOptions, &halOptConsoleForeground);
	option_add(&halOptions, &halOptConsoleBackground);
	option_add(&halOptions, &halOptConsoleHighlight);
	option_add(&halOptions, &halOptConsoleBoldtext);
	
	option_add(&halOptions, &halOptKeyboardLog);
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
		// printf("\n[Unhandled interrupt: %d]\n", cpu->interrupt);
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

/*
struct multiboot_mmap_entry
{
 multiboot_uint32_t size;
 multiboot_uint64_t addr;
 multiboot_uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
 multiboot_uint32_t type;
} __attribute__((packed));
*/

extern const void kernel_start;
extern const void kernel_end;

static void init_pmm(struct multiboot_info const * info)
{
	if((info->flags & MULTIBOOT_INFO_MEM_MAP) == 0)
		return;
	
	// hal_debug("Reading mmap...\n");
	
	uint32_t ptr = info->mmap_addr;
	for(uint32_t i = 0; i < info->mmap_length; i++) {
		struct multiboot_mmap_entry *mmap = (void*)ptr;
		
		/*
		hal_debug(
			"[%d] = { .size=%d, .addr=%d, .len=%d, .type=%d }\n",
			i,
			(uint32_t)mmap->size,
			(uint32_t)mmap->addr,
			(uint32_t)mmap->len,
			mmap->type);
		*/
		
		if((mmap->type == MULTIBOOT_MEMORY_AVAILABLE || mmap->type == MULTIBOOT_MEMORY_RESERVED)
			&& mmap->addr < PMM_MEMSIZE)
		{
			int marking = PMM_USED;
			if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
				marking = PMM_FREE;
			for(uint64_t offset = 0; offset < mmap->len; offset += PMM_PAGESIZE)
			{
				uint64_t addr = mmap->addr + offset;
				if(addr >= PMM_MEMSIZE) continue;
				
				page_t page = pmm_getpage((void*)addr);
				
				pmm_mark(page, marking);
			}
		}
		ptr += mmap->size + 0x04;
	}
	// hal_debug("End of mmap.\n");
	
	for(char const *ptr = &kernel_start; ptr <= (char const*)&kernel_end; ptr += 0x1000)
	{
		pmm_mark(pmm_getpage(ptr), PMM_USED);
	}
}
