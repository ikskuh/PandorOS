#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct GDT
{
	uint32_t base;
	uint32_t limit;
	uint8_t type;
};

struct GDT gdt[3];

void kerror(char const *msg){
	fprintf(stderr, "Kernel Error: %s\n", msg);
	exit(1);
}

/**
 * \param target A pointer to the 8-byte GDT entry
 * \param source An arbitrary structure describing the GDT entry
 */
void encodeGdtEntry(uint8_t *target, struct GDT source)
{
    // Check the limit to make sure that it can be encoded
    if ((source.limit > 65536) && (source.limit & 0xFFF) != 0xFFF) {
        kerror("You can't do that!");
    }
    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}

int main(int argc, char **argv)
{
	gdt[0] = (struct GDT){.base=0, .limit=0, .type=0};                     // Selector 0x00 cannot be used
	gdt[1] = (struct GDT){.base=0, .limit=0xffffffff, .type=0x9A};         // Selector 0x08 will be our code
	gdt[2] = (struct GDT){.base=0, .limit=0xffffffff, .type=0x92};         // Selector 0x10 will be our data
	
	uint64_t list[3];
	
	encodeGdtEntry((uint8_t*)&list[0], gdt[0]);
	encodeGdtEntry((uint8_t*)&list[1], gdt[1]);
	encodeGdtEntry((uint8_t*)&list[2], gdt[2]);
	
	for(int i = 0; i < 3; i++) {
		fprintf(stdout, "\t0x%016llX,\n", list[i]);
	
	}
	
	return 0;
}