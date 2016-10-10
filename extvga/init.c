#include <stdint.h>
#include "multiboot.h"

struct multiboot_header __attribute__((section ("multiboot"))) multibootHeader __attribute__ ((aligned (4))) = {
	MULTIBOOT_HEADER_MAGIC, 
	0x00,
	-(MULTIBOOT_HEADER_MAGIC + 0x00),
	
	// These are only valid if MULTIBOOT_AOUT_KLUDGE is set. 
	0, // header_addr;
	0, // load_addr;
	0, // load_end_addr;
	0, // bss_end_addr;
	0, // entry_addr;

	// These are only valid if MULTIBOOT_VIDEO_MODE is set.
	0, // mode_type;
	0, // width;
	0, // height;
	0, // depth;
};

void write_regs(unsigned char *regs);
void read_regs(unsigned char *regs);
void set_text_mode(int hi_res);

extern unsigned char g_80x25_text[];
extern unsigned char g_80x50_text[];

void init()
{
	const char hw[] = "Hello World!";
	int i;
	char* video = (char*) 0xb8000;

	// C-Strings haben ein Nullbyte als Abschluss
	for (i = 0; hw[i] != '\0'; i++) {

		// Zeichen i in den Videospeicher kopieren
		video[i * 2] = hw[i];

		// 0x07 = Hellgrau auf Schwarz
		video[i * 2 + 1] = 0x07;
	}
	
	// write_regs(g_80x50_text);
	set_text_mode(1);
	
	while(1);
}